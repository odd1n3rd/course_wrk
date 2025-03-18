#ifndef MTP_DEVICE_MANAGER_H
#define MTP_DEVICE_MANAGER_H

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <libmtp.h>

// Предварительное объявление классов
class MtpDevice;

/**
 * @brief Менеджер MTP-устройств
 * 
 * Класс отвечает за обнаружение подключенных MTP-устройств
 * и управление их жизненным циклом.
 */
class MtpDeviceManager {
public:
    /**
     * Конструктор
     */
    MtpDeviceManager();

    /**
     * Деструктор
     */
    ~MtpDeviceManager();

    /**
     * @brief Инициализирует libmtp
     * @return true в случае успеха, false в случае ошибки
     */
    bool initialize();

    /**
     * @brief Освобождает ресурсы libmtp
     */
    void shutdown();

    /**
     * @brief Обнаруживает подключенные MTP-устройства
     * @return true если обнаружены устройства, false в противном случае
     */
    bool detectDevices();

    /**
     * @brief Возвращает количество обнаруженных устройств
     * @return Количество устройств
     */
    size_t getDeviceCount() const;

    /**
     * @brief Возвращает устройство по индексу
     * @param index Индекс устройства
     * @return Умный указатель на устройство или nullptr, если индекс некорректен
     */
    std::shared_ptr<MtpDevice> getDevice(size_t index) const;

    /**
     * @brief Возвращает список всех устройств
     * @return Вектор умных указателей на устройства
     */
    std::vector<std::shared_ptr<MtpDevice>> getAllDevices() const;

    /**
     * @brief Тип функции обратного вызова для уведомлений о изменениях в устройствах
     */
    using DeviceChangeCallback = std::function<void()>;

    /**
     * @brief Регистрирует функцию обратного вызова для уведомлений об изменениях
     * @param callback Функция обратного вызова
     * @return ID зарегистрированного обратного вызова
     */
    int registerDeviceChangeCallback(DeviceChangeCallback callback);

    /**
     * @brief Удаляет функцию обратного вызова по ID
     * @param callbackId ID функции обратного вызова
     * @return true если функция обратного вызова успешно удалена
     */
    bool unregisterDeviceChangeCallback(int callbackId);

    /**
     * @brief Возвращает последнее сообщение об ошибке
     * @return Строка с сообщением об ошибке
     */
    std::string getLastError() const;

private:
    /**
     * @brief Освобождает все обнаруженные устройства
     */
    void clearDevices();

    /**
     * @brief Вызывает все зарегистрированные функции обратного вызова
     */
    void notifyDeviceChange();

private:
    bool m_initialized;                               ///< Флаг инициализации libmtp
    LIBMTP_raw_device_t* m_rawDevices;                ///< Массив сырых устройств libmtp
    int m_rawDeviceCount;                             ///< Количество сырых устройств
    std::vector<std::shared_ptr<MtpDevice>> m_devices; ///< Список MTP-устройств
    std::string m_lastError;                          ///< Последнее сообщение об ошибке
    std::vector<std::pair<int, DeviceChangeCallback>> m_callbacks; ///< Список функций обратного вызова
    int m_nextCallbackId;                             ///< ID для следующей функции обратного вызова
    mutable std::mutex m_mutex;                       ///< Мьютекс для потокобезопасности
};

#endif // MTP_DEVICE_MANAGER_H