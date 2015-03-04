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
    private Font normalFont = new Font( "Normal", Font.BOLD, 15 );

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
        //slider.addChangeListener( this );
        //add( slider, BorderLayout.EAST );
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
            
            g.setColor( Color.LIGHT_GRAY );
            g.setFont( normalFont );

            double w = source.orientValues[ 0 ];
            double x = source.orientValues[ 1 ];
            double y = source.orientValues[ 2 ];
            double z = source.orientValues[ 3 ];

            g.drawString( "w = " + w, 11, 35 );
            g.drawString( "x = " + x, 15, 50 );
            g.drawString( "y = " + y, 15, 65 );
            g.drawString( "z = " + z, 15, 80 );

            double[] gravity = { 2 * ( x*z - w*y ), 2 * ( w*x + y*z ), w*w - x*x - y*y + z*z };

            g.drawString( "Pitch: " + Math.toRadians( Math.atan( x / Math.sqrt( y*y + z*z ) ) ), 15, 100 );
            g.drawString( "Roll:  " + Math.toRadians( Math.atan( y / Math.sqrt( x*x + z*z ) ) ), 15, 115 );
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






