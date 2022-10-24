/* MacToolTitlebar::MacToolTitlebar(QWidget *parent)
    :QMacNativeWidget(NULL),
     m_title(new QLabel(parent)),
     m_toolbarItem(NULL),
     m_proxyicon_filler(new QWidget(this)),
     m_proxyicon_host(NULL)
{
    m_parentWidget = parent;
    setFocusPolicy(Qt::NoFocus);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    QPalette p = m_title->palette();
    p.setColor(QPalette::WindowText, QColor::fromRgb(77,73,77));
    m_title->setPalette(p);
    m_title->setMinimumWidth(5);
    m_title->setMaximumHeight(18);
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_title->setAlignment(Qt::AlignTop|Qt::AlignCenter);
    m_proxyicon_filler->setMaximumSize(1,1);
    m_proxyicon_filler->setVisible(false);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_proxyicon_filler);
    layout->addWidget(m_title);
    setLayout(layout);
    m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    m_title->setText(tr("Untitled"));
    m_title->show();
}

void MacToolTitlebar::setTitle(QString title)
{
    m_title->setText(title);
    // do some herustics here
    // if we have an invalid file, use string in full
    // otherwise use the actual name of the file.
    if(QFile::exists(title))
    {
        m_filePath = title;
        QFileInfo file(title);
        m_title->setText(file.fileName());
        createProxyIcon();
    }
    else
    {
        m_filePath = QString();
        if(m_proxyicon_host != NULL)
            destroyProxyIcon();
    }
}

QString MacToolTitlebar::title()
{
    return m_title->text();
}

void MacToolTitlebar::insertInToolbar(QMacToolBar *t)
{
    NSToolbarItem *ti = t->addItem(QIcon(""),
                                   QString(""))->nativeToolBarItem();

    [ti setView:nativeView()];
    m_toolbarItem = ti;
    setVisible(true);

    NSSize size;
    size.height = 50;
    size.width = 300;
    [ti setMaxSize:size];
    [ti autorelease];
}

void MacToolTitlebar::createProxyIcon()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    EditorWindow *wnd = qobject_cast<EditorWindow*>(m_parentWidget);
    NSWindow *window = (NSWindow*)
            QGuiApplication::platformNativeInterface()->nativeResourceForWindow("nswindow", wnd->windowHandle());

    NSButton *btn = [window standardWindowButton:NSWindowDocumentIconButton];
    [btn removeFromSuperview];
    if(btn != nil)
    {
        m_proxyicon_host = new QMacCocoaViewContainer(btn, this);
        m_proxyicon_host->setMaximumSize(QSize(16,16));
        layout()->replaceWidget(m_proxyicon_filler, m_proxyicon_host);
    }

    [btn release];
    [pool release];
}

void MacToolTitlebar::destroyProxyIcon()
{
    layout()->replaceWidget(m_proxyicon_host, m_proxyicon_filler);
} */
