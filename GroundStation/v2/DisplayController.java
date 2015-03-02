import javax.swing.*;
import java.awt.event.*;

class DisplayController.java extends JPanel implements ActionListener {

    private MessageContent source = new MessageContent();

    public DisplayController() {
        setBackground( Color.DARK_GRAY );
        setPerferredSize( new Dimension( 0, 100 ) );
    }

    public void paintComponent( Graphics g ) {
        super.paintComponent( g );
        
        g.setColor( Color.LIGHT_GRAY );

    }

    /**
     *
     */
    public void actionPerformed( ActionEvent evt ) {
        redraw();
    }
}