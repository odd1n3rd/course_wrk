#include "MtpStorage.h"
#include "MtpFile.h"
#include "MtpDirectory.h"
#include <iostream>

MtpStorage::MtpStorage(LIBMTP_mtpdevice_t* device, LIBMTP_devicestorage_t* storage)
    : m_device(device)
    , m_storage(storage)
{
}

MtpStorage::~MtpStorage()
{
    // Не освобождаем m_storage, так как он будет освобожден
    // при освобождении устройства
}

uint32_t MtpStorage::getId() const
{
    return m_storage->id;
}

std::string MtpStorage::getDescription() const
{
    if (m_storage->StorageDescription) {
        return std::string(m_storage->StorageDescription);
    }
    return "Unknown Storage";
}

uint64_t MtpStorage::getMaxCapacity() const
{
    return m_storage->MaxCapacity;
}

uint64_t MtpStorage::getFreeSpace() const
{
    return m_storage->FreeSpaceInBytes;
}

std::shared_ptr<MtpDirectory> MtpStorage::getRootDirectory()
{
    // Создаем корневую директорию с ID 0
    return std::make_shared<MtpDirectory>(m_device, 0, getId(), "Root");
}

std::shared_ptr<MtpFile> MtpStorage::getFileById(uint32_t fileId)
{
    LIBMTP_file_t* file = LIBMTP_Get_Filemetadata(m_device, fileId);
    
    if (!file) {
        m_lastError = "File not found";
        return nullptr;
    }
    
    // Создаем объект MtpFile или MtpDirectory в зависимости от типа
    std::shared_ptr<MtpFile> result;
    if (file->filetype == LIBMTP_FILETYPE_FOLDER) {
        result = std::make_shared<MtpDirectory>(m_device, fileId, getId(), file->filename);
    } else {
        result = std::make_shared<MtpFile>(m_device, file, getId());
    }
    
    // Освобождаем файловую структуру libmtp
    LIBMTP_destroy_file_t(file);
    
    return result;
}

std::vector<std::shared_ptr<MtpFile>> MtpStorage::getFiles(uint32_t parentId)
{
    std::vector<std::shared_ptr<MtpFile>> files;
    
    LIBMTP_file_t* fileList = LIBMTP_Get_Files_And_Folders(m_device, getId(), parentId);
    
    if (!fileList) {
        // Проверяем на ошибки
        LIBMTP_error_t* error = LIBMTP_Get_Errorstack(m_device);
        if (error) {
            m_lastError = error->error_text;
            LIBMTP_Clear_Errorstack(m_device);
        } else {
            m_lastError = "No files found";
        }
        return files;
    }
    
    // Итерируемся по списку файлов
    LIBMTP_file_t* current = fileList;
    while (current) {
        if (current->filetype == LIBMTP_FILETYPE_FOLDER) {
            files.push_back(std::make_shared<MtpDirectory>(m_device, current->item_id, getId(), current->filename));
        } else {
            files.push_back(std::make_shared<MtpFile>(m_device, current, getId()));
        }
        current = current->next;
    }
    
    // Освобождаем список файлов
    LIBMTP_destroy_file_t(fileList);
    
    return files;
}

uint32_t MtpStorage::createDirectory(const std::string& name, uint32_t parentId)
{
    uint32_t newFolderId = LIBMTP_Create_Folder(m_device, name.c_str(), parentId, getId());
    
    if (newFolderId == 0) {
        // Проверяем на ошибки
        LIBMTP_error_t* error = LIBMTP_Get_Errorstack(m_device);
        if (error) {
            m_lastError = error->error_text;
            LIBMTP_Clear_Errorstack(m_device);
        } else {
            m_lastError = "Failed to create directory";
        }
    }
    
    return newFolderId;
}

bool MtpStorage::deleteObject(uint32_t id)
{
    int ret = LIBMTP_Delete_Object(m_device, id);
    
    if (ret != 0) {
        // Проверяем на ошибки
        LIBMTP_error_t* error = LIBMTP_Get_Errorstack(m_device);
        if (error) {
            m_lastError = error->error_text;
            LIBMTP_Clear_Errorstack(m_device);
        } else {
            m_lastError = "Failed to delete object";
        }
        return false;
    }
    
    return true;
}

std::string MtpStorage::getLastError() const
{
    return m_lastError;
}