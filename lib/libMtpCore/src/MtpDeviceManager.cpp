#include "MtpDeviceManager.h"
#include "MtpDevice.h"
#include <algorithm>
#include <iostream>

MtpDeviceManager::MtpDeviceManager()
    : m_initialized(false)
    , m_rawDevices(nullptr)
    , m_rawDeviceCount(0)
    , m_nextCallbackId(1)
{
}

MtpDeviceManager::~MtpDeviceManager()
{
    shutdown();
}

bool MtpDeviceManager::initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        return true;
    }
    
    // Инициализируем библиотеку libmtp
    LIBMTP_Init();
    m_initialized = true;
    
    // Попробуем сразу обнаружить устройства
    return detectDevices();
}

void MtpDeviceManager::shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }
    
    // Очищаем список устройств
    clearDevices();
    
    // Освобождаем сырые устройства, если они были получены
    if (m_rawDevices) {
        free(m_rawDevices);
        m_rawDevices = nullptr;
        m_rawDeviceCount = 0;
    }
    
    m_initialized = false;
}

bool MtpDeviceManager::detectDevices()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        m_lastError = "MTP library not initialized";
        return false;
    }
    
    // Очищаем текущий список устройств
    clearDevices();
    
    // Освобождаем предыдущий список сырых устройств, если он существует
    if (m_rawDevices) {
        free(m_rawDevices);
        m_rawDevices = nullptr;
        m_rawDeviceCount = 0;
    }
    
    // Получаем список сырых устройств
    int ret = LIBMTP_Detect_Raw_Devices(&m_rawDevices, &m_rawDeviceCount);
    
    if (ret != LIBMTP_ERROR_NONE) {
        switch (ret) {
            case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
                m_lastError = "No devices found";
                return false;
            case LIBMTP_ERROR_CONNECTING:
                m_lastError = "Error connecting to device";
                return false;
            case LIBMTP_ERROR_MEMORY_ALLOCATION:
                m_lastError = "Memory allocation error";
                return false;
            default:
                m_lastError = "Unknown error: " + std::to_string(ret);
                return false;
        }
    }
    
    // Проверяем, есть ли устройства
    if (m_rawDeviceCount <= 0) {
        m_lastError = "No devices found";
        return false;
    }
    
    // Создаем объекты устройств на основе сырых устройств
    for (int i = 0; i < m_rawDeviceCount; i++) {
        // Открываем устройство с помощью libmtp
        LIBMTP_mtpdevice_t* mtpDevice = LIBMTP_Open_Raw_Device_Uncached(&m_rawDevices[i]);
        
        if (mtpDevice) {
            // Создаем объект MtpDevice и добавляем его в список
            std::shared_ptr<MtpDevice> device = std::make_shared<MtpDevice>(mtpDevice, m_rawDevices[i]);
            m_devices.push_back(device);
        } else {
            std::cerr << "Failed to open device at index " << i << std::endl;
        }
    }
    
    // Уведомляем об изменении списка устройств
    notifyDeviceChange();
    
    return !m_devices.empty();
}

size_t MtpDeviceManager::getDeviceCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_devices.size();
}

std::shared_ptr<MtpDevice> MtpDeviceManager::getDevice(size_t index) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (index >= m_devices.size()) {
        return nullptr;
    }
    
    return m_devices[index];
}

std::vector<std::shared_ptr<MtpDevice>> MtpDeviceManager::getAllDevices() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_devices;
}

int MtpDeviceManager::registerDeviceChangeCallback(DeviceChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Присваиваем текущий ID и увеличиваем счетчик
    int callbackId = m_nextCallbackId++;
    
    // Добавляем функцию обратного вызова в список
    m_callbacks.push_back(std::make_pair(callbackId, callback));
    
    return callbackId;
}

bool MtpDeviceManager::unregisterDeviceChangeCallback(int callbackId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Ищем функцию обратного вызова с указанным ID
    auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(),
                          [callbackId](const std::pair<int, DeviceChangeCallback>& pair) {
                              return pair.first == callbackId;
                          });
    
    // Если нашли, удаляем и возвращаем true
    if (it != m_callbacks.end()) {
        m_callbacks.erase(it);
        return true;
    }
    
    return false;
}

std::string MtpDeviceManager::getLastError() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lastError;
}

void MtpDeviceManager::clearDevices()
{
    // Очищаем вектор устройств
    m_devices.clear();
    
    // Устройства libmtp будут освобождены деструкторами MtpDevice
}

void MtpDeviceManager::notifyDeviceChange()
{
    // Создаем локальную копию списка функций обратного вызова,
    // чтобы избежать блокировки мьютекса во время вызова
    std::vector<DeviceChangeCallback> callbacks;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& pair : m_callbacks) {
            callbacks.push_back(pair.second);
        }
    }
    
    // Вызываем все функции обратного вызова
    for (const auto& callback : callbacks) {
        callback();
    }
}