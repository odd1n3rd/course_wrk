#ifndef MTP_DEVICE_H
#define MTP_DEVICE_H

#include <string>
#include <vector>
#include <memory>
#include <libmtp.h>

// Предварительное объявление классов
class MtpStorage;

/**
 * @brief Представление MTP-устройства
 * 
 * Класс представляет отдельное MTP-устройство и предоставляет
 * методы для получения информации о нем и работы с ним.
 */
class MtpDevice {
public:
    /**
     * @brief Конструктор
     * @param device Указатель на устройство libmtp
     * @param rawDevice Структура сырого устройства libmtp
     */
    MtpDevice(LIBMTP_mtpdevice_t* device, LIBMTP_raw_device_t rawDevice);

    /**
     * @brief Деструктор
     */
    ~MtpDevice();

    /**
     * @brief Получает название устройства
     * @return Строка с названием устройства
     */
    std::string getFriendlyName() const;

    /**
     * @brief Получает производителя устройства
     * @return Строка с названием производителя
     */
    std::string getManufacturer() const;

    /**
     * @brief Получает модель устройства
     * @return Строка с названием модели
     */
    std::string getModelName() const;

    /**
     * @brief Получает серийный номер устройства
     * @return Строка с серийным номером
     */
    std::string getSerialNumber() const;

    /**
     * @brief Получает версию прошивки устройства
     * @return Строка с версией прошивки
     */
    std::string getDeviceVersion() const;

    /**
     * @brief Получает версию MTP-протокола, поддерживаемую устройством
     * @return Строка с версией MTP
     */
    std::string getMtpVersion() const;

    /**
     * @brief Обновляет список хранилищ устройства
     * @return true в случае успеха, false в случае ошибки
     */
    bool updateStorages();

    /**
     * @brief Получает количество хранилищ на устройстве
     * @return Количество хранилищ
     */
    size_t getStorageCount() const;

    /**
     * @brief Получает хранилище по индексу
     * @param index Индекс хранилища
     * @return Умный указатель на хранилище или nullptr, если индекс некорректен
     */
    std::shared_ptr<MtpStorage> getStorage(size_t index) const;

    /**
     * @brief Получает список всех хранилищ устройства
     * @return Вектор умных указателей на хранилища
     */
    std::vector<std::shared_ptr<MtpStorage>> getAllStorages() const;

    /**
     * @brief Получает последнее сообщение об ошибке
     * @return Строка с сообщением об ошибке
     */
    std::string getLastError() const;

    /**
     * @brief Получает указатель на устройство libmtp
     * @return Указатель на устройство libmtp
     */
    LIBMTP_mtpdevice_t* getLibMtpDevice() const;

private:
    LIBMTP_mtpdevice_t* m_device;                        ///< Указатель на устройство libmtp
    LIBMTP_raw_device_t m_rawDevice;                     ///< Структура сырого устройства libmtp
    std::vector<std::shared_ptr<MtpStorage>> m_storages;  ///< Список хранилищ устройства
    mutable std::string m_lastError;                     ///< Последнее сообщение об ошибке
};

#endif // MTP_DEVICE_H