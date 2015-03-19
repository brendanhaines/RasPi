import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

class DisplayController extends JPanel implements ActionListener {

    private MessageContent source = new MessageContent();
    private Font normalFont = new Font( "Normal", Font.BOLD, 15 );

    public DisplayController() {
        setBackground( Color.DARK_GRAY );
        setPreferredSize( new Dimension( 0, 100 ) );
    }

    public DisplayController( MessageContent content ) {
        this();
        source = content;
    }

    /**
     *
     */
    /*private int scaleChan( int chan, int range ) {
        return (int)(range * ( ( (double)(chan - chanMin) / (chanMax - chanMin)  ) - 0.5 ));
    }*/

    public void paintComponent( Graphics g ) {
        super.paintComponent( g );
        
        int centerX = getWidth() / 2;
        int centerY = getHeight() / 2;

        int boxSide = getWidth() / 3;
        int circleDiam = boxSide / 10;
        int circleRad = circleDiam / 2;

        g.setColor( Color.LIGHT_GRAY );
        g.fillRect( boxSide / 4, boxSide / 4, boxSide , boxSide );
        g.fillRect( boxSide * 7/4 , boxSide / 4, boxSide , boxSide );
        g.setColor( Color.BLACK );
        g.drawRect( boxSide / 4, boxSide / 4, boxSide , boxSide );
        g.drawRect( boxSide * 7/4 , boxSide / 4, boxSide , boxSide );

        g.setColor( Color.RED );
        // left stick
        g.fillOval(
            source.controlValues[ 3 ] * boxSide / 1000 - circleRad + boxSide * 1/4,
            source.controlValues[ 0 ] * boxSide / 1000 - circleRad + boxSide * 11/4,
            circleDiam,
            circleDiam );
        // right stick
        g.fillOval(
            source.controlValues[ 1 ] * boxSide / 1000 - circleRad + boxSide * 7/4,
            source.controlValues[ 2 ] * boxSide / 1000 - circleRad + boxSide * 11/4,
            circleDiam,
            circleDiam );

        g.setColor( Color.LIGHT_GRAY );
        for( int i = 0; i < source.controlValues.length; i++ ) {
            //g.drawString( "Channel " + i + ": " + source.controlValues[0], 15, boxSide * 3/2 + i * 15 );
            if( i %3 == 0 ) {
                g.drawString( "Channel " + i + ": " + source.controlValues[0], 15, boxSide * 3/2 + i/3 * 15 );
            }
            else if( i %3 == 1) {
                g.drawString( "Channel " + i + ": " + source.controlValues[0], 150 + 15, boxSide * 3/2 + i/3 * 15 );
            }
            else {
                g.drawString( "Channel " + i + ": " + source.controlValues[0], 300 + 15, boxSide * 3/2 + i/3 * 15 );
            }
        }
    }

    /**
     *
     */
    public void actionPerformed( ActionEvent evt ) {
        repaint();
    }
}