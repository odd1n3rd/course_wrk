#ifndef MTP_STORAGE_H
#define MTP_STORAGE_H

#include <string>
#include <memory>
#include <vector>
#include <libmtp.h>

// Предварительное объявление классов
class MtpFile;
class MtpDirectory;

/**
 * @brief Представление хранилища MTP-устройства
 * 
 * Класс представляет хранилище на MTP-устройстве и предоставляет
 * методы для работы с ним.
 */
class MtpStorage {
public:
    /**
     * @brief Конструктор
     * @param device Указатель на устройство libmtp
     * @param storage Указатель на хранилище libmtp
     */
    MtpStorage(LIBMTP_mtpdevice_t* device, LIBMTP_devicestorage_t* storage);

    /**
     * @brief Деструктор
     */
    ~MtpStorage();

    /**
     * @brief Получает ID хранилища
     * @return ID хранилища
     */
    uint32_t getId() const;

    /**
     * @brief Получает описание хранилища
     * @return Строка с описанием хранилища
     */
    std::string getDescription() const;

    /**
     * @brief Получает общий объем хранилища
     * @return Общий объем в байтах
     */
    uint64_t getMaxCapacity() const;

    /**
     * @brief Получает свободный объем хранилища
     * @return Свободный объем в байтах
     */
    uint64_t getFreeSpace() const;

    /**
     * @brief Получает корневую директорию хранилища
     * @return Умный указатель на корневую директорию
     */
    std::shared_ptr<MtpDirectory> getRootDirectory();

    /**
     * @brief Получает файл по его ID
     * @param fileId ID файла
     * @return Умный указатель на файл или nullptr, если файл не найден
     */
    std::shared_ptr<MtpFile> getFileById(uint32_t fileId);

    /**
     * @brief Получает список файлов в указанной директории
     * @param parentId ID родительской директории (0 для корневой директории)
     * @return Вектор умных указателей на файлы
     */
    std::vector<std::shared_ptr<MtpFile>> getFiles(uint32_t parentId = 0);

    /**
     * @brief Создает новую директорию
     * @param name Имя новой директории
     * @param parentId ID родительской директории (0 для корневой директории)
     * @return ID созданной директории или 0 в случае ошибки
     */
    uint32_t createDirectory(const std::string& name, uint32_t parentId = 0);

    /**
     * @brief Удаляет файл или директорию
     * @param id ID файла или директории
     * @return true в случае успеха, false в случае ошибки
     */
    bool deleteObject(uint32_t id);

    /**
     * @brief Получает последнее сообщение об ошибке
     * @return Строка с сообщением об ошибке
     */
    std::string getLastError() const;

private:
    LIBMTP_mtpdevice_t* m_device;         ///< Указатель на устройство libmtp
    LIBMTP_devicestorage_t* m_storage;    ///< Указатель на хранилище libmtp
    mutable std::string m_lastError;      ///< Последнее сообщение об ошибке
};

#endif // MTP_STORAGE_H