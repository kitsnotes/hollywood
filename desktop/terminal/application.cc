#include "application.h"
#include "window.h"
#include "profile.h"

#include <QIcon>
#include <QSettings>
#include <QCommandLineParser>
#include <hollywood/hollywood.h>

TerminalApplication::TerminalApplication(int &argc, char **argv)
    :QApplication(argc, argv)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setApplicationName("Terminull");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setWindowIcon(QIcon::fromTheme("utilities-terminal"));
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setAttribute(Qt::AA_DontShowIconsInMenus, true);
    loadProfiles();
}

QList<TerminalProfile *> TerminalApplication::profiles()
{
    return m_profiles;
}

TerminalProfile *TerminalApplication::profileByName(const QString &name)
{
    for(auto p : m_profiles)
        if(p->profileName() == name)
            return p;

    return nullptr;
}

QString TerminalApplication::defaultProfileName()
{
    QSettings settings("originull","terminull");
    auto defProfile = settings.value("DefaultProfile").toString();

    if(profileByName(defProfile))
        return defProfile;

    // if we get here the default is not a profile
    auto fixprofile = m_profiles.first()->profileName();
    settings.setValue("DefaultProfile", fixprofile);

    return fixprofile;
}

bool TerminalApplication::createProfile(const QString &name)
{
    QSettings settings("originull","terminull");
    settings.beginGroup(QLatin1String("Profile-%1").arg(name.trimmed()));
    if(!settings.childKeys().isEmpty())
        return false;

    auto profile = new TerminalProfile();
    profile->setProfileName(name);
    profile->saveProfileToSettings();
    m_profiles.append(profile);
    return true;
}

bool TerminalApplication::removeProfile(const QString &name)
{
    // if we are default, change it
    QSettings settings("originull","terminull");
    if(name == defaultProfileName())
    {
        for(auto group : settings.childGroups())
        {
            if(group.startsWith(QLatin1String("Profile-")))
            {
                if(group != QString("Profile-%1").arg(name))
                {
                    auto name = group.remove("Profile-");
                    settings.setValue("DefaultProfile", name);
                }
            }
        }
    }

    // signal our windows (to signal our tabs) this profile is going away
    emit profileRemoved(name);

    // remove the profile from memory
    for(auto profile : m_profiles)
        if(profile->profileName() == name)
            m_profiles.removeOne(profile);

    // remove the profile from settings
    settings.remove(QLatin1String("Profile-%1").arg(name));

    return true;
}

void TerminalApplication::newWindow()
{
    auto *w = new TerminalWindow();
    m_windows.append(w);
    w->show();
}

void TerminalApplication::windowClosed()
{
    auto *w = qobject_cast<TerminalWindow*>(sender());
    if(m_windows.contains(w))
        m_windows.removeOne(w);

    w->deleteLater();
}

void TerminalApplication::loadProfiles()
{
    QSettings settings("originull", "terminull");
    for(auto group : settings.childGroups())
    {
        if(group.startsWith(QLatin1String("Profile-")))
        {
            auto profile = new TerminalProfile();
            auto name = group.remove(0, 8);
            if(profile->loadProfileFromSettings(name))
                m_profiles.append(profile);
            else
                profile->deleteLater();
        }
    }

    if(m_profiles.isEmpty())
    {
        // oh snap - let's put in a default profile
        auto profile = new TerminalProfile();
        profile->setProfileName(tr("Basic"));
        profile->saveProfileToSettings();
        loadProfiles();
    }
}

int main(int argc, char *argv[])
{
    TerminalApplication a(argc, argv);
    QCommandLineParser p;
    p.setApplicationDescription("Originull Terminal Emulator");
    p.addHelpOption();
    p.addVersionOption();
    p.process(a);

    a.newWindow();
    return a.exec();
}
