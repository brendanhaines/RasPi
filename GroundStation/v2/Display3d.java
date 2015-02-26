import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

/**
 *
 */
class Display3d extends JPanel implements ActionListener, ChangeListener {

    private JSlider slider;
    private MessageContent source = new MessageContent();
    private Font normalFont = new Font( "Normal", Font.PLAIN, 15 );

    /**
     * no-args constructor
     */
    public Display3d() {
        setBackground( Color.DARK_GRAY );
        setPreferredSize( new Dimension( 500, 100 ) );
    }

    /**
     * 
     */
    public Display3d( MessageContent orientationSource ) {
        source = orientationSource;
        source.addActionListener( this );

        setBackground( Color.DARK_GRAY );
        setPreferredSize( new Dimension( 0, 100 ) );

        setLayout( new BorderLayout() );
        slider = new JSlider();
        slider.setOrientation( JSlider.VERTICAL );
        slider.addChangeListener( this );
        add( slider, BorderLayout.EAST );
    }

    /**
     * overrides super.paintComponent
     */
    public void paintComponent( Graphics g ) {
        super.paintComponent( g );

        int centerX = getWidth() / 2;
        int centerY = getHeight() / 2;

        slider.setEnabled( source.orientation );

        if( source.orientation ) {

            // yaw axis
            g.setColor( Color.BLUE );
            //g.drawLine( centerX, centerY, , );

            g.setColor( Color.RED );
            //g.drawLine( centerX, centerY, X, Y );
            
            g.setColor( Color.GREEN );
            //g.drawLine( centerX, centerY, X, Y );
            
            g.setColor( Color.BLACK );
            g.setFont( normalFont );
            g.drawString( "w = " + source.orientValues[ 0 ], 10, 35 );
            g.drawString( "x = " + source.orientValues[ 1 ], 10, 60 );
            g.drawString( "y = " + source.orientValues[ 2 ], 10, 85 );
            g.drawString( "z = " + source.orientValues[ 3 ], 10, 110 );
        }
        else {
            g.setColor( Color.RED );
            g.setFont( new Font( "Alert", Font.BOLD, 30 ) );
            g.drawString( "NO ORIENTATION DATA", centerX - 180, centerY + 15 );
        }
    }

    /**
     *
     */
    public void actionPerformed( ActionEvent evt ) {
        repaint();
    }

    /**
     *
     */
    public void stateChanged( ChangeEvent evt ) {
        repaint();
    }
}






