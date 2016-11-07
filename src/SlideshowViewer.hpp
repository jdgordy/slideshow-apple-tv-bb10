/*
 * SlideshowViewer.hpp
 *
 *  Created on: Apr 20, 2013
 *      Author: jgordy
 */

#ifndef SLIDESHOWVIEWER_HPP_
#define SLIDESHOWVIEWER_HPP_

#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QThread>
#include <QtNetwork/QHostAddress>
#include "dns_sd.h"

// Forward class declarations
class AirPlayConnection;
class AirPlayCommandExecutor;
class AirPlayDevice;

//
// SlideshowViewer
//
class SlideshowViewer : public QObject
{
    Q_OBJECT

    // The currently selected device
    Q_PROPERTY(AirPlayDevice* device READ device NOTIFY deviceChanged);

    // The currently displayed picture
    Q_PROPERTY(QVariant currentPicture READ currentPicture NOTIFY currentPictureChanged);

    // The data properties of the record that is displayed
    Q_PROPERTY(QString status READ status NOTIFY statusChanged);
    Q_PROPERTY(bool operationPending READ operationPending NOTIFY operationPendingChanged);
    Q_PROPERTY(bool hasNextPicture READ hasNextPicture NOTIFY hasNextPictureChanged);
    Q_PROPERTY(bool hasPreviousPicture READ hasPreviousPicture NOTIFY hasPreviousPictureChanged);

public:

    // Constants
    static const int SLIDESHOW_STATE_ERROR = -1;
    static const int SLIDESHOW_STATE_STOPPED = 0;
    static const int SLIDESHOW_STATE_STARTING = 1;
    static const int SLIDESHOW_STATE_PAUSED = 2;
    static const int SLIDESHOW_STATE_SENDING_PHOTO = 3;
    static const int SLIDESHOW_STATE_STOPPING = 4;

    // Constructor / destructor
    SlideshowViewer(QObject* pParent = NULL);
    virtual ~SlideshowViewer();

    // Set the device and password
    void setDevice(AirPlayDevice* pDevice, const QString& password);

    // Set slide show properties: file list, transition, and image scaling
    void setSlideshowProperties(const QVariantList& fileList, int transition, int screenWidth, int screenHeight, int quality, bool scaleImages);

    // Start the slideshow
    Q_INVOKABLE bool startSlideshow();

    // Stop the slideshow
    Q_INVOKABLE bool stopSlideshow();

    // Show next picture
    Q_INVOKABLE bool showNextPicture();

    // Show previous picture
    Q_INVOKABLE bool showPreviousPicture();

    // Cancel operation in progress
    Q_INVOKABLE bool cancelOperation();

Q_SIGNALS:

    // Property change notification signals
    void deviceChanged();
    void currentPictureChanged();
    void statusChanged();
    void operationPendingChanged();
    void hasNextPictureChanged();
    void hasPreviousPictureChanged();

    // Emitted on operation completion
    void operationComplete();

    // Emitted on operation cancellation
    void operationCancelled();

    // Emitted on operation error
    void error(const QString& errorDescription);

protected Q_SLOTS:

    // Handler for command status
    void handleCommandStatus(const QString& status);

    // Handler for command completion
    void handleCommandsComplete();

    // Handler for command cancellation
    void handleCommandsCancelled();

    // Handler for error
    void handleError(const QString& errorDescription);

protected:

    // Property accessor methods
    AirPlayDevice* device() const;
    QVariant currentPicture() const;
    QString status() const;
    bool operationPending() const;
    bool hasNextPicture() const;
    bool hasPreviousPicture() const;

    // Property values
    QString m_status;
    bool m_bOperationPending;
    bool m_bHasNextPicture;
    bool m_bHasPreviousPicture;

    // AirPlay connection and command executor instances
    AirPlayConnection*      m_pConnection;
    AirPlayCommandExecutor* m_pExecutor;

    // AirPlay device instance
    AirPlayDevice*  m_pDevice;
    QString         m_password;

    // Slideshow information
    QVariantList    m_fileList;
    int             m_currentFileIndex;
    int             m_transition;
    int             m_state;
};

#endif /* SLIDESHOWVIEWER_HPP_ */
