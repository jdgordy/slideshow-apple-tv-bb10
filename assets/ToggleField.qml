import bb.cascades 1.0

Container {
    
    // Define property aliases
    property alias text: labelField.text
    property alias checked: toggleField.checked

    // Arrange controls left-to-right
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    
    // Set padding / margins
    topPadding: 10
    bottomPadding: 10
    leftPadding: 10
    rightPadding: 10

    Label {
        
        // Set properties
        id: labelField

        // Use remaining space
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        
        // Center vertically
        verticalAlignment: VerticalAlignment.Center
    }
    
    ToggleButton {
        
        // Set properties
        id: toggleField
        
        // Use required space
        layoutProperties: StackLayoutProperties {
            spaceQuota: -1
        }
        
        // Center vertically
        verticalAlignment: VerticalAlignment.Center
    }
}
