#include "operationthread.h"

#include <sys/file.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>

OperationWorker::OperationWorker(const QList<OpItem> &sources, const OperationManager::OperationType type, const QUrl target, QObject *parent)
    :QObject(parent)
    ,m_sources(sources)
    ,m_optype(type)
    ,m_targetPath(target)
{
    m_readarchive << "tar"
                  << "pax"
                  << "cpio"
                  << "iso"
                  << "zip"
                  << "mtree"
                  << "ar"
                  << "xar"
                  << "rar"
                  << "cab"
                  << "7z";

    m_writearchive << "tar"
                   << "pax"
                   << "cpio"
                   << "iso"
                   << "zip"
                   << "mtree"
                   << "ar"
                   << "xar"
                   << "7z";

}

void OperationWorker::startWorker()
{
    // See if either our source/dest operations involve libarchive
    auto src = m_sources.first();
    if(m_readarchive.contains(src.u.scheme().toLower()))
        m_source_archive = true;

    if(m_writearchive.contains(m_targetPath.scheme().toLower()))
        m_dest_archive = true;

    // See if we have permission to write to the target

    // Prepare our files - this will:
    //    - calculate our needed space
    //    - emit any permissions errors on source files
    prepareFiles();

    // Check to make sure we have ample space on the target
    if(m_optype == OperationManager::Copy ||
        (m_optype == OperationManager::Move && moveCrossingFilesystem()))
    {
        auto cost = copyDataCost();

    }

    doWork();
}

void OperationWorker::prepareFiles()
{
    for(auto i : m_sources)
    {
        if(i.u.scheme() == "file")
        {
            struct stat s;
            auto f = i.u.toLocalFile().toUtf8().data();
            if(lstat(f, &s))
            {
                // gather up some basic information
                i.size = s.st_size;
                i.inode = s.st_ino;
                i.gid = s.st_gid;
                i.uid = s.st_uid;
            }
        }
    }
}

void OperationWorker::doWork()
{
    // Do our action
    if(m_optype == OperationManager::Copy)
        fsCopyOperation();
    else if(m_optype == OperationManager::Move)
        fsMoveOperation();
    else if(m_optype == OperationManager::Symlink)
        fsSymlinkOperation();
    else if(m_optype == OperationManager::Trash)
        fsTrashOperation();
}

void OperationWorker::fsMoveOperation()
{
    // "Move" does not work in libarchives
}

void OperationWorker::fsCopyOperation()
{
    for(auto f : m_sources)
    {
        if(m_targetPath.scheme() == "file")
        {
            bool success = false;
            if(f.u.scheme() == "file")
            {
                // FS to FS copy

                // See if we're on the same filesystem
                if(!fileCrossFilesystem(f.u))
                {
                    //rename()
                }
                success = true;
            }
            if(success)
            {
                //m_sources.removeOne(f);
                //m_processed.append(f);
            }
        }
        else
        {
            // Handle a target of archive
        }
    }
}

void OperationWorker::fsSymlinkOperation()
{
    // "Symlink" does not work in libarchives
}

void OperationWorker::fsDeleteOperation()
{

}

void OperationWorker::fsTrashOperation()
{
    // "Trash" does not work in libarchives
    for(auto f : m_sources)
    {
        if(m_targetPath.scheme() == "file")
        {
            bool success = false;
            if(f.u.scheme() == "file")
            {
                QFile file(f.u.toLocalFile());
                success = file.moveToTrash();
                if(success)
                {
                    //m_sources.removeOne(f);
                    //m_processed.append(f);
                }
                else
                {
                    // Had an error trashing the file,
                    // throw an error and stop

                    return;
                }
            }
        }
        else
        {
            // requested from archive, emit an error
        }
    }
}

void OperationWorker::archiveCopyOperation()
{

}

void OperationWorker::archiveDeleteOperation()
{

}

bool OperationWorker::moveCrossingFilesystem()
{

    return false;
}

quint64 OperationWorker::copyDataCost()
{
    return 0;
}

bool OperationWorker::fileCrossFilesystem(const QUrl &url)
{
    return true;
}
