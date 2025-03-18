#include "MtpDevice.h"
#include "MtpStorage.h"
#include <algorithm>
#include <iostream>

MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* device, LIBMTP_raw_device_t rawDevice)
    : m_device(device)
    , m_rawDevice(rawDevice)
{
    // Обновляем список хранилищ при создании объекта
    updateStorages();
}

MtpDevice::~MtpDevice()
{
    if (m_device) {
        LIBMTP_Release_Device(m_device);
        m_device = nullptr;
    }
}

std::string MtpDevice::getFriendlyName() const
{
    char* name = LIBMTP_Get_Friendlyname(m_device);
    if (name) {
        std::string result(name);
        free(name);
        return result;
    }
    return "Unknown Device";
}

std::string MtpDevice::getManufacturer() const
{
    char* manufacturer = LIBMTP_Get_Manufacturername(m_device);
    if (manufacturer) {
        std::string result(manufacturer);
        free(manufacturer);
        return result;
    }
    return "Unknown Manufacturer";
}

std::string MtpDevice::getModelName() const
{
    char* model = LIBMTP_Get_Modelname(m_device);
    if (model) {
        std::string result(model);
        free(model);
        return result;
    }
    return "Unknown Model";
}

std::string MtpDevice::getSerialNumber() const
{
    char* serial = LIBMTP_Get_Serialnumber(m_device);
    if (serial) {
        std::string result(serial);
        free(serial);
        return result;
    }
    return "Unknown Serial";
}

std::string MtpDevice::getDeviceVersion() const
{
    char* version = LIBMTP_Get_Deviceversion(m_device);
    if (version) {
        std::string result(version);
        free(version);
        return result;
    }
    return "Unknown Version";
}

std::string MtpDevice::getMtpVersion() const
{
    uint16_t major = m_rawDevice.device_version >> 8;
    uint16_t minor = m_rawDevice.device_version & 0xFF;
    return std::to_string(major) + "." + std::to_string(minor);
}

bool MtpDevice::updateStorages()
{
    if (!m_device) {
        m_lastError = "Device not initialized";
        return false;
    }

    // Очищаем текущий список хранилищ
    m_storages.clear();

    // Получаем список хранилищ с устройства
    LIBMTP_devicestorage_t* storageList = LIBMTP_Get_Storage(m_device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
    
    if (!storageList) {
        m_lastError = "Failed to get storage list";
        return false;
    }

    // Итерируемся по списку хранилищ и создаем объекты MtpStorage
    LIBMTP_devicestorage_t* current = storageList;
    while (current) {
        std::shared_ptr<MtpStorage> storage = std::make_shared<MtpStorage>(m_device, current);
        m_storages.push_back(storage);
        current = current->next;
    }

    // Не освобождаем storageList, так как это будет сделано при освобождении устройства

    return !m_storages.empty();
}

size_t MtpDevice::getStorageCount() const
{
    return m_storages.size();
}

std::shared_ptr<MtpStorage> MtpDevice::getStorage(size_t index) const
{
    if (index >= m_storages.size()) {
        return nullptr;
    }
    
    return m_storages[index];
}

std::vector<std::shared_ptr<MtpStorage>> MtpDevice::getAllStorages() const
{
    return m_storages;
}

std::string MtpDevice::getLastError() const
{
    return m_lastError;
}

LIBMTP_mtpdevice_t* MtpDevice::getLibMtpDevice() const
{
    return m_device;
}