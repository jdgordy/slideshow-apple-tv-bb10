import bb.cascades 1.0

// Use a NavigationPane to hold contents
NavigationPane {
    
    // Set properties
    id: mainNavigationPane
    peekEnabled: false
    
    // Handler for pop transition
    onPopTransitionEnded: {
        
        // Enable the application menu
        Application.menuEnabled = true
        
        // Delete the viewer page
        page.destroy();
    }
    
    // Create application menu    
    Menu.definition: MenuDefinition {

        // Create help action
        helpAction: HelpActionItem {

            // Add handler to display help page
            onTriggered: {

                // Disable the application menu
                Application.menuEnabled = false;

                // Create and display help page
                var helpPage = helpPageDefinition.createObject();
                mainNavigationPane.push(helpPage);
            }
        }
        
        // Create settings action
        settingsAction: SettingsActionItem {

            // Add handler to display settings page
            onTriggered: {

                // Disable the application menu
                Application.menuEnabled = false;

                // Create and display settings page
                var settingsPage = settingsPageDefinition.createObject();
                mainNavigationPane.push(settingsPage);
            }
        }
        
        // Create remaining actions
        actions: [
            
            ActionItem {
                
                // Set properties
                id: infoActionItem
                imageSource: "asset:///images/ic_info.png"
                title: "Info"
                
                // Add handler to display settings page
                onTriggered: {

                    // Disable the application menu
                    Application.menuEnabled = false;

                    // Create and display info page
                    var infoPage = infoPageDefinition.createObject();
                    mainNavigationPane.push(infoPage);
                }
            }
        ]
    }
    
    Page {
        
        // Set properties
        id: mainPage
        
        // Create title bar
        titleBar: TitleBar {
            title: qsTr("Slideshow Setup")
        }
        
        // Create a container to hold all controls
        Container {
            
            // Arrange children top-to-bottom
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            
            // Set padding / margins
            topPadding: 10
            bottomPadding: 10
            leftPadding: 10
            rightPadding: 10
            
            // Create the device name field
            ViewerField {
                
                // Set properties
                title: "Device:"
                value: (mainApp.device != null) ? mainApp.device.name : "None"
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }

            // Create a container to hold password controls
            Container {

                // Arrange children top-to-bottom
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }

                // Set padding / margins
                topPadding: 10
                bottomPadding: 10
                leftPadding: 10
                rightPadding: 10

                // Visibility is conditional on device
                visible: ((mainApp.device != null) && mainApp.device.passwordRequired) ? true : false

                Label {
                    
                    // Set properties
                    text: "Password:"
                    textStyle {
                        base: SystemDefaults.TextStyles.PrimaryText
                    }
                
                    // Fill horizontally
                    horizontalAlignment: HorizontalAlignment.Fill
                }
                
                // Create the password field
                TextField {
                
                    // Set properties
                    id: passwordText
                    hintText: "Password"
                    inputMode: TextFieldInputMode.Password
                    
                    // Fill horizontally
                    horizontalAlignment: HorizontalAlignment.Fill
                    
                    // Handler for text change
                    onTextChanged: {
                        
                        // Set application parameter
                        mainApp.setPassword(text);
                    }
                }
            }
            
            // Create the pictures field
            ViewerField {
                
                // Set properties
                title: "Pictures:"
                value: mainApp.fileListModel.size() + " selected";

                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
            }
            
            // Add divider between fields and drop-down(s)
            Divider {
            }
            
            // Create dropdown for transition
            DropDown {
                
                // Set properties
                id: transitionOption
                title: "Transition"
                selectedIndex: 0
                
                // Fill horizontally
                horizontalAlignment: HorizontalAlignment.Fill
                
                Option {
                    text: "None"
                    value: 0
                }
                
                Option {
                    text: "Slide left"
                    value: 1
                }
                
                Option {
                    text: "Slide right"
                    value: 2
                }
                
                Option {
                    text: "Dissolve"
                    value: 3
                }
                
                // Handler for index change
                onSelectedIndexChanged: {

                    // Set application parameter
                    mainApp.setTransition(selectedIndex);
                }
            }
        }
        
        actions: [
            
            // Device picker action
            ActionItem {
                
                // Set properties
                title: "Devices"
                imageSource: "asset:///images/tv-action-icon.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                // Handler for trigger
                onTriggered: {
                    
                    // Display the device viewer
                    deviceViewer.open();
                }
            },
            
            // Picture organize action
            ActionItem {
                
                // Set properties
                title: "Pictures"
                imageSource: "asset:///images/photo-action-icon.png"
                ActionBar.placement: ActionBarPlacement.Signature
                
                // Handler for trigger
                onTriggered: {
                    
                    // Display the file viewer
                    fileViewer.open();
                }
            },
            
            // Start slideshow action
            ActionItem {
                
                // Set properties
                title: "Start"
                imageSource: "asset:///images/ic_play.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                
                // Connect enable flag to device instance
                enabled: (mainApp.device != null) ? true : false;
                
                // Handler for trigger
                onTriggered: {

                    // Configure the slideshow
                    mainApp.configureSlideshow();
                    
                    // Display the slideshow viewer
                    slideshowViewer.open();
                }
            }
        ]
    }
    
    attachedObjects: [
        
        // Help page definition            
        ComponentDefinition {
            
            // Set properties
            id: helpPageDefinition
            source: "HelpPage.qml"
        },

        // Settings page definition
        ComponentDefinition {

            // Set properties
            id: settingsPageDefinition
            source: "SettingsPage.qml"
        },

        // Info page definition
        ComponentDefinition {

            // Set properties
            id: infoPageDefinition
            source: "InfoPage.qml"
        },

        // Create device viewer
        DeviceViewer {
            
            // Set properties
            id: deviceViewer
        },
        
        // Create file viewer
        FileViewer {
            
            // Set properties
            id: fileViewer
        },
        
        // Create slideshow viewer
        SlideshowViewer {
            
            // Set properties
            id: slideshowViewer
            slideshowViewer: mainApp.slideshowViewer
        }
    ]
}
