import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class ArmButton extends JButton implements ActionListener {
    private MessageContent content;

    public ArmButton( MessageContent newContent ) {
        super( "Arm Flight Controller" );
        content = newContent;
        setAlignmentX(Component.CENTER_ALIGNMENT);
        addActionListener( this );
    }

    public void actionPerformed( ActionEvent evt ) {
        if( getText().equals( "Arm Flight Controller" ) ) {
            setText( "Disarm Flight Controller" );
            content.setMotorsEnabled( true );
        }
        else {
            setText( "Arm Flight Controller" );
            content.setMotorsEnabled( false );
        }
    }
}