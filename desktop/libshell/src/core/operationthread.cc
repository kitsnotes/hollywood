#include "operationthread.h"

#include <sys/file.h>

OperationWorker::OperationWorker(const QList<OpItem> &sources, const OperationType type, const QUrl target, QObject *parent)
    :QThread(parent)
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

void OperationWorker::run()
{
    //QMutexLocker ml(&m_mutex);
    //ml.lock();

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
    if(m_optype == Copy ||
        (m_optype == Move && moveCrossingFilesystem()))
    {
        auto cost = copyDataCost();
    }

    // Do our action
    if(m_optype == Copy)
        fsCopyOperation();
    else if(m_optype == Move)
        fsMoveOperation();
    else if(m_optype == Symlink)
        fsSymlinkOperation();
    else if(m_optype == Trash)
        fsTrashOperation();
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

void OperationWorker::fsMoveOperation()
{

}

void OperationWorker::fsCopyOperation()
{

}

void OperationWorker::fsSymlinkOperation()
{

}

void OperationWorker::fsDeleteOperation()
{

}

void OperationWorker::fsTrashOperation()
{

}

void OperationWorker::archiveCopyOperation()
{

}

void OperationWorker::archiveDeleteOperation()
{

}

bool OperationWorker::moveCrossingFilesystem()
{

}

quint64 OperationWorker::copyDataCost()
{

}
