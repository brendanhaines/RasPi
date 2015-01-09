/**
 * Written By Brendan Haines
 */

class TCPclientExample {
    public static void main( String args[] ) {
        TCPclient client = new TCPclient( "192.168.42.1", 51719 );
        System.out.println( "Setup successful" );
        client.close();
    }
}