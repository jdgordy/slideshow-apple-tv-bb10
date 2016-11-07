import bb.cascades 1.0

// Create the main page
Page {

    // Set properties

    // Create title bar
    titleBar: TitleBar {

        // Set properties
        title: "Settings"
    }

    // Handler for creation completed
    onCreationCompleted: {

        // Load the application parameters
        connectionTimeoutOption.setSelectedIndex(connectionTimeoutOption.findIndex(mainApp.getParameter("ConnectionTimeout", "20000")));
        scaleImagesToggle.checked = mainApp.getParameter("EnableScaleImages", "false");
        screenSizeOption.setSelectedIndex(screenSizeOption.findIndex(mainApp.getParameter("ScreenSize", "720p")));
        imageQualityOption.setSelectedIndex(imageQualityOption.findIndex(mainApp.getParameter("ImageQuality", "high")));
    }
    
    // Create a container to hold all controls
    Container {

        // Arrange children top-to-bottom
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }

        // Set some padding for child controls
        topPadding: 10
        bottomPadding: 10
        leftPadding: 10
        rightPadding: 10

        // Connection timeout drop-down
        DropDown {

            // Set properties
            id: connectionTimeoutOption
            title: "Connection Timeout"

            // Fill horizontally
            horizontalAlignment: HorizontalAlignment.Fill

            Option {
                text: "10 sec."
                value: 10000
            }

            Option {
                text: "20 sec."
                value: 20000
            }

            Option {
                text: "30 sec."
                value: 30000
            }

            Option {
                text: "1 min."
                value: 60000
            }

            // Handler for option change
            onSelectedValueChanged: {
                
                // Set the application parameter
                mainApp.setParameter("ConnectionTimeout", selectedValue.toString());
            }
            
            // Map from option value to selected index
            function findIndex(value) {
                value = parseInt(value);
                for (var i = 0; i < count(); i ++) {
                    if (at(i).value == value) return i;
                }
                return i - 1; // default
            }
        }
        
        // Scale oversize images toggle
        ToggleField {
            
            // Set properties
            id: scaleImagesToggle
            text: "Scale Oversize Images"

            // Fill horizontally
            horizontalAlignment: HorizontalAlignment.Fill
            
            // Handler for checked
            onCheckedChanged: {

                // Set the application parameter
                mainApp.setParameter("EnableScaleImages", checked);
            }
        }
        
        // Screen size drop-down
        DropDown {

            // Set properties
            id: screenSizeOption
            title: "Screen Size"
            enabled: scaleImagesToggle.checked
            
            // Fill horizontally
            horizontalAlignment: HorizontalAlignment.Fill

            Option {
                text: "720p (1280x720)"
                value: "720p"
            }

            Option {
                text: "1080p (1920x1080)"
                value: "1080p"
            }

            // Handler for option change
            onSelectedValueChanged: {

                // Set the application parameter
                mainApp.setParameter("ScreenSize", selectedValue);
            }

            // Map from option value to selected index
            function findIndex(value) {
                for (var i = 0; i < count(); i ++) {
                    if (at(i).value == value) return i;
                }
                return i - 1; // default
            }
        }
        
        // Image quality drop-down
        DropDown {
            
            // Set properties
            id: imageQualityOption
            title: "Image Quality"
            enabled: scaleImagesToggle.checked
            
            // Fill horizontally
            horizontalAlignment: HorizontalAlignment.Fill
            
            Option {
                text: "Low"
                value: "low"
            }
            
            Option {
                text: "Medium"
                value: "medium"
            }
            
            Option {
                text: "High"
                value: "high"
            }
            
            
            // Handler for option change
            onSelectedValueChanged: {
                
                // Set the application parameter
                mainApp.setParameter("ImageQuality", selectedValue);
            }
            
            // Map from option value to selected index
            function findIndex(value) {
                for (var i = 0; i < count(); i ++) {
                    if (at(i).value == value) return i;
                }
                return i - 1; // default
            }
        }
    }
}
