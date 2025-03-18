#ifndef MTP_FILE_H
#define MTP_FILE_H

#include <string>
#include <libmtp.h>
#include <memory>
#include <vector>

/**
 * @brief Представление файла на MTP-устройстве
 * 
 * Класс представляет файл на MTP-устройстве и предоставляет
 * методы для работы с ним.
 */
class MtpFile {
public:
    /**
     * @brief Конструктор
     * @param device Указатель на устройство libmtp
     * @param file Указатель на файл libmtp
     * @param storageId ID хранилища
     */
    MtpFile(LIBMTP_mtpdevice_t* device, LIBMTP_file_t* file, uint32_t storageId);

    /**
     * @brief Виртуальный деструктор
     */
    virtual ~MtpFile();

    /**
     * @brief Получает ID файла
     * @return ID файла
     */
    uint32_t getId() const;

    /**
     * @brief Получает ID родительской директории
     * @return ID родительской директории
     */
    uint32_t getParentId() const;

    /**
     * @brief Получает ID хранилища
     * @return ID хранилища
     */
    uint32_t getStorageId() const;

    /**
     * @brief Получает имя файла
     * @return Имя файла
     */
    std::string getName() const;

    /**
     * @brief Получает размер файла
     * @return Размер файла в байтах
     */
    uint64_t getSize() const;

    /**
     * @brief Проверяет, является ли объект директорией
     * @return true если объект - директория, false в противном случае
     */
    virtual bool isDirectory() const;

    /**
     * @brief Скачивает файл на компьютер
     * @param path Путь для сохранения файла
     * @return true в случае успеха, false в случае ошибки
     */
    bool downloadFile(const std::string& path);

    /**
     * @brief Удаляет файл с устройства
     * @return true в случае успеха, false в случае ошибки
     */
    bool deleteFile();

    /**
     * @brief Получает последнее сообщение об ошибке
     * @return Строка с сообщением об ошибке
     */
    std::string getLastError() const;

protected:
    LIBMTP_mtpdevice_t* m_device;     ///< Указатель на устройство libmtp
    uint32_t m_id;                    ///< ID файла
    uint32_t m_parentId;              ///< ID родительской директории
    uint32_t m_storageId;             ///< ID хранилища
    std::string m_name;               ///< Имя файла
    uint64_t m_size;                  ///< Размер файла
    mutable std::string m_lastError;  ///< Последнее сообщение об ошибке
};

#endif // MTP_FILE_H