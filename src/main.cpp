// Default empty project template
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <Qt/QDeclarativeDebug.h>
#include <QtCore/QTimer>
#include "AirPlaySlideshow.hpp"
#include "AirPlayDevice.hpp"
#include "DevicePropertiesViewer.hpp"
#include "SlideshowViewer.hpp"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    // Register our custom types with QML, so that they can be used as property types
    qmlRegisterType<AirPlayDevice>();
    qmlRegisterType<DevicePropertiesViewer>();
    qmlRegisterType<SlideshowViewer>();
    qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");

    // this is where the server is started etc
    Application app(argc, argv);

    // localization support
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "AirPlaySlideshow_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    // Create application instance
    new AirPlaySlideshow(&app);

    // Start the application event loop
    return Application::exec();
}
