import bb.cascades 1.0

Container {
    
    // Define property aliases
    property alias title: titleField.text
    property alias value: valueField.text
    
    // Arrange controls left-to-right
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }

    // Set padding / margins
    topPadding: 10
    bottomPadding: 10
    leftPadding: 10
    rightPadding: 10

    // Define the title field
    Label {
        
        // Set properties
        id: titleField
        textStyle {
            base: SystemDefaults.TextStyles.PrimaryText
        }
        
        // Use required space
        layoutProperties: StackLayoutProperties {
            spaceQuota: -1
        }
        
        // Center vertically
        verticalAlignment: VerticalAlignment.Center
    }
    
    // Define the value field 
    Label {
        
        // Set properties
        id: valueField
        multiline: true
        textStyle {
            base: SystemDefaults.TextStyles.PrimaryText
            textAlign: TextAlign.Right
            fontStyle: FontStyle.Italic
        }
        
        // Use remaining space
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        
        // Fill vertically
        verticalAlignment: VerticalAlignment.Fill
    }
}
