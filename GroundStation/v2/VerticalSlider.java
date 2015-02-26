import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

class VerticalSlider extends JPanel implements ActionListener, ChangeListener {

    /** up button */
    private JButton up;
    /** slider bar */
    private JSlider slider;
    /** down button */
    private JButton down;
    /** value field */
    private JTextField value;
    /** list to hold subscribed actionlisteners */
    private ArrayList<ActionListener> listeners;
    /** set to false for use fo setValue */
    private boolean internalTrigger = true;

    ////////////////////////
    ///// CONSTRUCTORS /////
    ////////////////////////

    /**
     * no-args constructor
     */
    public VerticalSlider() {
        setLayout( new BoxLayout( this, BoxLayout.PAGE_AXIS ) );
        setLabel( "" );


        up = new JButton( "\u25B2" );
        slider = new JSlider();
        slider.setOrientation( JSlider.VERTICAL );
        slider.setValue( 0 );
        down = new JButton( "\u25BC" );
        
        value = new JTextField( "0" );

        up.addActionListener( this );
        down.addActionListener( this );
        value.addActionListener( this );

        slider.addChangeListener( this );

        this.add( up );
        this.add( slider );
        this.add( down );
        this.add( value );

        listeners = new ArrayList<ActionListener>();
    }

    /**
     * sets label of slider on construction
     * @param newLabel - label to put on slider
     */
    public VerticalSlider( String newLabel ) {
        this();
        setLabel( newLabel );
    }

    /**
     * sets slider extremes
     * also sets slider value to minimum
     * @param sliderMin - minimum extreme value of slider
     * @param sliderMax - maximum extreme value of slider
     */
    public VerticalSlider( int sliderMin, int sliderMax ) {
        this();
        slider.setMinimum( sliderMin );
        slider.setMaximum( sliderMax );
        setValue( sliderMin );
    }

    /**
     * sets label of slider and sets slider extremes
     * also sets slider value to minimum
     * @param newLabel - label to put on slider
     * @param sliderMin - minimum extreme value of slider
     * @param sliderMax - maximum extreme value of slider
     */
    public VerticalSlider( String newLabel, int sliderMin, int sliderMax ) {
        this( sliderMin, sliderMax );
        setLabel( newLabel );
    }

    ///////////////////
    ///// METHODS /////
    ///////////////////

    /**
     * @return the value of the slider
     */
    public int getValue() {
        return slider.getValue();
    }

    /**
     * @param newVal - value to set slider to
     */
    public void setValue( int newVal ) {
        internalTrigger = false;
        slider.setValue( newVal );  // will trigger changeListener and update everything else
        internalTrigger = true;
    }

    /**
     * changes the slider's label
     * @param newLabel - the new name to put on slider
     */
    public void setLabel( String newLabel ) {
        setBorder( BorderFactory.createTitledBorder( newLabel ) );
    }

    /**
     * implementation of actionlistener interface
     */
    public void actionPerformed( ActionEvent evt ) {
        if( evt.getSource() == up ) {
            slider.setValue( slider.getValue() + 1 );
        }
        else if( evt.getSource() == down ) {
            slider.setValue( slider.getValue() - 1 );
        }
        else if( evt.getSource() == value ) {
            slider.setValue( Integer.parseInt( value.getText() ) );
        }
    }

    /**
     * run whenever slider changes position
     * implementation of changelistener interface
     */
    public void stateChanged( ChangeEvent evt ) {
        if( evt.getSource() == slider ) {
            value.setText( ""+ slider.getValue() );
            if( internalTrigger ) {
                fireActionPerformed();
            }
        }
    }

    /**
     *
     */
    public void setEnabled( boolean enable ) {
        up.setEnabled( enable );
        slider.setEnabled( enable );
        down.setEnabled( enable );
        value.setEnabled( enable );
    }

    /**
     * adds actionlistener to subscribd listener list
     * @param al - actionlistener to add
     */
    public void addActionListener( ActionListener al ) {
        listeners.add( al );
    }

    /**
     * removes actionlistener from subscribed listener list
     * @param al - actionlistener to remove
     */
    public void removeActionListener( ActionListener al ) {
        listeners.remove( al );
    }

    /**
     * sends an actionevent to subscribed listeners
     * @param ae - actionevent to use as parameter to all subscribed listeners
     */
    public void fireActionPerformed( ActionEvent ae ) {
        for( ActionListener al : listeners ) {
            al.actionPerformed( ae );
        }
    }

    /**
     *
     */
    public void fireActionPerformed() {
        fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, ""+ slider.getValue() ) );
    }
}






