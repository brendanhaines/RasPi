import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 *
 */
class MotorTestPanel extends JPanel implements ActionListener {

    /** checkBox to enable/disable motor testing */
    private JCheckBox checkBox;
    /** panel to hold sliders */
    private JPanel sliderPanel;

    /** slider for motor 1 */
    public VerticalSlider slider0;
    /** slider for motor 2 */
    public VerticalSlider slider1;
    /** slider for motor 3 */
    public VerticalSlider slider2;
    /** slider for motor 4 */
    public VerticalSlider slider3;
    /** slider for all motors */
    private VerticalSlider masterSlider;

    /** */
    public MessageContent controlOutput = new MessageContent();

    /** list to hold subscribed actionlisteners */
    private ArrayList<ActionListener> listeners;

    /**
     *
     */
    public MotorTestPanel() {
        listeners = new ArrayList<ActionListener>();

        this.setLayout( new BorderLayout() );
        checkBox = new JCheckBox( "Enable Motor Testing" );
        checkBox.addActionListener( this );
        this.add( checkBox, BorderLayout.NORTH );

        sliderPanel = new JPanel( new FlowLayout() );
        slider0 = new VerticalSlider( "Motor 0" );
        slider1 = new VerticalSlider( "Motor 1" );
        slider2 = new VerticalSlider( "Motor 2" );
        slider3 = new VerticalSlider( "Motor 3" );
        masterSlider = new VerticalSlider( "Master" );

        slider0.addActionListener( this );
        slider1.addActionListener( this );
        slider2.addActionListener( this );
        slider3.addActionListener( this );
        masterSlider.addActionListener( this );
        
        sliderPanel.add( slider0 );
        sliderPanel.add( slider1 );
        sliderPanel.add( slider2 );
        sliderPanel.add( slider3 );
        sliderPanel.add( masterSlider );

        this.add( sliderPanel, BorderLayout.CENTER );

        setTestStatus( false );
    }

    /**
     *
     */
    public MotorTestPanel( MessageContent controls ) {
        this();
        controlOutput = controls;
        controlOutput.addActionListener( this );
    }

    /**
     *
     */
    public void setEnabled( boolean state ) {
        if( !state ) {
            setTestStatus( false );
        }
        checkBox.setEnabled( state );
    }

    /**
     *
     */
    public void setTestStatus( boolean enable ) {
        checkBox.setSelected( enable );
        controlOutput.setMotorsEnabled( enable );
        controlOutput.motorTesting = enable;
        if( !enable ) {
            masterSlider.setValue( 0 );
            masterSlider.fireActionPerformed();
        }

        slider0.setEnabled( enable );
        slider1.setEnabled( enable );
        slider2.setEnabled( enable );
        slider3.setEnabled( enable );
        masterSlider.setEnabled( enable );
    }

    /**
     * @return true if testing is enabled
     */
    public boolean getTestStatus() {
        return checkBox.isSelected();
    }

    /**
     *
     */
    public int[] getValues() {
        int[] values = {slider0.getValue(),
                        slider1.getValue(),
                        slider2.getValue(),
                        slider3.getValue() };
        return values;
    }

    /**
     *
     */
    public void actionPerformed( ActionEvent evt ) {
        if( evt.getSource() == controlOutput ) {
            if( evt.getActionCommand().equals( "DISABLE_MOTORS" ) && getTestStatus() ) {
                setTestStatus( false );
            }
            return;
        }
        if( evt.getSource() == checkBox ) {
            setTestStatus( checkBox.isSelected() );
            return;
        }
        if( evt.getSource() == masterSlider ) {
            slider0.setValue( Integer.parseInt( evt.getActionCommand() ) );
            slider1.setValue( Integer.parseInt( evt.getActionCommand() ) );
            slider2.setValue( Integer.parseInt( evt.getActionCommand() ) );
            slider3.setValue( Integer.parseInt( evt.getActionCommand() ) );
        }
        fireActionPerformed();
    }

    ////////////////////////////////////////////

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
        //controlOutput.motorValues = { slider0.getValue(), slider1.getValue(), slider2.getValue(), slider3.getValue() };
        controlOutput.motorValues[0] = slider0.getValue();
        controlOutput.motorValues[1] = slider1.getValue();
        controlOutput.motorValues[2] = slider2.getValue();
        controlOutput.motorValues[3] = slider3.getValue();


        for( ActionListener al : listeners ) {
            al.actionPerformed( ae );
        }
    }

    /**
     *
     */
    public void fireActionPerformed() {
        fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "" ) );
    }
}