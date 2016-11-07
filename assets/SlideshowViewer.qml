import bb.cascades 1.0
import bb.system 1.0
import my.library 1.0

// Use a Sheet to hold controls
Sheet {

    // Define properties
    property variant slideshowViewer
    property bool bSlideshowStarting
    
    // Set properties
    id: slideshowViewerSheet
    peekEnabled: false
    
    // Handler for creation completed
    onCreationCompleted: {

        // Connect completion / cancelled signal
        slideshowViewer.operationComplete.connect(slideshowViewerPage.handleOperationComplete);
        slideshowViewer.operationCancelled.connect(slideshowViewerPage.handleOperationCancelled);
        slideshowViewer.error.connect(slideshowViewerPage.handleError);
    }
    
    // Handler for opened signal
    onOpened: {
        
        // Set the starting flag
        bSlideshowStarting = true;
        
        // Start slideshow
        if( slideshowViewer.startSlideshow() == false ) {
            // Close the sheet
            slideshowViewerSheet.close();

            // Display error toast
            systemToast.body = "Unable to start slideshow";
            systemToast.show();
        }
        else {
            // Start the timer for configured timeout
            var connectionTimeout = parseInt(mainApp.getParameter("ConnectionTimeout", "20000"));
            operationTimer.start(connectionTimeout);
            
            // Start the action bar timer
            actionBarTimer.start(5000);
        }
    }
    
    // Use a Page for content
    Page {
        
        // Set properties
        id: slideshowViewerPage
        actionBarVisibility: ChromeVisibility.Overlay
        
        // Helper function for completion signal
        function handleOperationComplete(){
            
            // Clear the starting flag
            bSlideshowStarting = false;
            
            // Stop the timer
            operationTimer.stop();
        }

        // Helper function for cancellation signal
        function handleOperationCancelled() {

            // Stop the timer
            operationTimer.stop();
            
            // Check if the slideshow was starting
            if( bSlideshowStarting ) {
                
                // Stop the slideshow
                slideshowViewer.stopSlideshow();
                
                // Close the sheet
                slideshowViewerSheet.close();

                // Display error toast
                systemToast.body = "Slideshow has been stopped.";
                systemToast.show();
            }
        }

        // Helper function for error signal
        function handleError(errorDescription) {
            
            // Stop the timer
            operationTimer.stop();
            
            // Stop the slideshow
            slideshowViewer.stopSlideshow();

            // Close the sheet
            slideshowViewerSheet.close();
            
            // Display error toast
            systemToast.body = "Slideshow has been stopped due to an unexpected error.\nReason: " + errorDescription;
            systemToast.show();
        }
        
        // Create a container to hold controls and activity indicator overlay
        Container {
            
            // Arrange children using docking
            layout: DockLayout {
            }
            
            // Create the picture preview
            ImageView {

                // Set properties
                imageSource: slideshowViewer.currentPicture.url
                scalingMethod: ScalingMethod.AspectFit
                loadEffect: ImageViewLoadEffect.Subtle

                // Fill horizontally and vertically
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                // Handler for touch events
                onTouch: {
                    
                    // Check for single event
                    if( event.touchType == TouchType.Down ) {
                        
                        // Toggle action bar visibility
                        if (slideshowViewerPage.actionBarVisibility == ChromeVisibility.Hidden) {
                            // Show the action bar and start timer
                            slideshowViewerPage.actionBarVisibility = ChromeVisibility.Overlay;
                            actionBarTimer.start(5000);
                        } else {
                            // Hide the action bar and cancel timer
                            slideshowViewerPage.actionBarVisibility = ChromeVisibility.Hidden;
                            actionBarTimer.stop();
                        }
                    }
                }
            }
            
            // Create activity indicator
            ActivityBar {

                // Set properties
                text: slideshowViewer.status
                
                // Visible while operation is pending
                visible: slideshowViewer.operationPending
                
                // Fill horizontally and align at the top
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Top
            }
        }
        
        actions: [
            
            // Create cancel action
            ActionItem {
                
                // Set properties
                title: "Stop"
                imageSource: "asset:///images/ic_cancel.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                // Condition on slideshow state
                enabled: !slideshowViewer.operationPending
                
                // Handler for trigger
                onTriggered: {
                    
                    // Stop the slideshow
                    slideshowViewer.stopSlideshow();
                    
                    // Close the sheet
                    slideshowViewerSheet.close();
                    
                    // Display toast
                    systemToast.body = "Slideshow has been stopped.";
                    systemToast.show();
                }
            },
            
            // Create previous action
            ActionItem {
                
                // Set properties
                title: "Previous"
                imageSource: "asset:///images/ic_previous.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                // Condition on slideshow state
                enabled: (slideshowViewer.hasPreviousPicture && !slideshowViewer.operationPending)
                
                // Handler for trigger
                onTriggered: {

                    // Show previous picture
                    if( slideshowViewer.showPreviousPicture() == false ) {
                        // Display error toast
                        systemToast.body = "Unable to display previous picture";
                        systemToast.show();
                    }
                    else {
                        // Start the timer for configured timeout
                        var connectionTimeout = parseInt(mainApp.getParameter("ConnectionTimeout", "20000"));
                        operationTimer.start(connectionTimeout);
                    }
                }
            },
            
            // Create start slideshow
            ActionItem {
                
                // Set properties
                title: "Next"
                imageSource: "asset:///images/ic_next.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                // Condition on slideshow state
                enabled: (slideshowViewer.hasNextPicture && !slideshowViewer.operationPending)
                
                // Handler for trigger
                onTriggered: {

                    // Show next picture
                    if( slideshowViewer.showNextPicture() == false ) {
                        // Display error toast
                        systemToast.body = "Unable to display next picture";
                        systemToast.show();
                    }
                    else {
                        // Start the timer for configured timeout
                        var connectionTimeout = parseInt(mainApp.getParameter("ConnectionTimeout", "20000"));
                        operationTimer.start(connectionTimeout);
                    }
                }
            }
        ]
        
        attachedObjects: [

            // Progress dialog
            SystemToast {

                // Set properties
                id: systemToast
                body: ""
            },
            
            // Timer (operations)
            QTimer {
                
                // Set properties
                id: operationTimer
                singleShot: true
                
                // Handler for timeout
                onTimeout: {
                    
                    // Cancel pending operations
                    slideshowViewer.cancelOperation();
                    
                    // Display error toast
                    systemToast.body = "Operation cancelled; device is taking too long to respond.";
                    systemToast.show();
                }
            },

            // Timer (action bar)
            QTimer {

                // Set properties
                id: actionBarTimer
                singleShot: true

                // Handler for timeout
                onTimeout: {

                    // Hide the action bar
                    slideshowViewerPage.actionBarVisibility = ChromeVisibility.Hidden;
                }
            }
        ]
    }
}
