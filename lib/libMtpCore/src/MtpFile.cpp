#include "MtpFile.h"
#include <iostream>

MtpFile::MtpFile(LIBMTP_mtpdevice_t* device, LIBMTP_file_t* file, uint32_t storageId)
    : m_device(device)
    , m_id(file->item_id)
    , m_parentId(file->parent_id)
    , m_storageId(storageId)
    , m_name(file->filename ? file->filename : "")
    , m_size(file->filesize)
{
}

MtpFile::~MtpFile()
{
}

uint32_t MtpFile::getId() const
{
    return m_id;
}

uint32_t MtpFile::getParentId() const
{
    return m_parentId;
}

uint32_t MtpFile::getStorageId() const
{
    return m_storageId;
}

std::string MtpFile::getName() const
{
    return m_name;
}

uint64_t MtpFile::getSize() const
{
    return m_size;
}

bool MtpFile::isDirectory() const
{
    return false;
}

bool MtpFile::downloadFile(const std::string& path)
{
    int ret = LIBMTP_Get_File_To_File(m_device, m_id, path.c_str(), nullptr, nullptr);
    
    if (ret != 0) {
        // Проверяем на ошибки
        LIBMTP_error_t* error = LIBMTP_Get_Errorstack(m_device);
        if (error) {
            m_lastError = error->error_text;
            LIBMTP_Clear_Errorstack(m_device);
        } else {
            m_lastError = "Failed to download file";
        }
        return false;
    }
    
    return true;
}

bool MtpFile::deleteFile()
{
    int ret = LIBMTP_Delete_Object(m_device, m_id);
    
    if (ret != 0) {
        // Проверяем на ошибки
        LIBMTP_error_t* error = LIBMTP_Get_Errorstack(m_device);
        if (error) {
            m_lastError = error->error_text;
            LIBMTP_Clear_Errorstack(m_device);
        } else {
            m_lastError = "Failed to delete file";
        }
        return false;
    }
    
    return true;
}

std::string MtpFile::getLastError() const
{
    return m_lastError;
}