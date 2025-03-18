#ifndef MTP_DIRECTORY_H
#define MTP_DIRECTORY_H

#include "MtpFile.h"
#include <vector>
#include <memory>

/**
 * @brief Представление директории на MTP-устройстве
 * 
 * Класс наследуется от MtpFile и представляет директорию на
 * MTP-устройстве, добавляя методы для работы с содержимым директории.
 */
class MtpDirectory : public MtpFile {
public:
    /**
     * @brief Конструктор
     * @param device Указатель на устройство libmtp
     * @param id ID директории
     * @param storageId ID хранилища
     * @param name Имя директории
     * @param parentId ID родительской директории
     */
    MtpDirectory(LIBMTP_mtpdevice_t* device, uint32_t id, uint32_t storageId, 
                 const std::string& name, uint32_t parentId = 0);

    /**
     * @brief Деструктор
     */
    virtual ~MtpDirectory();

    /**
     * @brief Проверяет, является ли объект директорией
     * @return true всегда, так как это директория
     */
    virtual bool isDirectory() const override;

    /**
     * @brief Получает содержимое директории
     * @return Вектор умных указателей на файлы и поддиректории
     */
    std::vector<std::shared_ptr<MtpFile>> getContent();

    /**
     * @brief Создает поддиректорию
     * @param name Имя новой директории
     * @return ID созданной директории или 0 в случае ошибки
     */
    uint32_t createDirectory(const std::string& name);

    /**
     * @brief Получает файл по имени
     * @param name Имя файла
     * @return Умный указатель на файл или nullptr, если файл не найден
     */
    std::shared_ptr<MtpFile> getFileByName(const std::string& name);

    /**
     * @brief Отправляет файл в директорию
     * @param localPath Локальный путь к файлу
     * @param remoteName Имя файла на устройстве (если пусто, используется имя локального файла)
     * @return ID созданного файла или 0 в случае ошибки
     */
    uint32_t sendFile(const std::string& localPath, const std::string& remoteName = "");
};

#endif // MTP_DIRECTORY_H