import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.Socket;

public class MyClient {
    public static void main(String[] args) throws Exception{
        Socket socket = new Socket("localhost",3333);
        DataInputStream din = new DataInputStream(socket.getInputStream());
        DataOutputStream dout = new DataOutputStream(socket.getOutputStream());
        BufferedReader buffRead = new BufferedReader(new InputStreamReader(System.in));

        String messageToServer = "";
        String messageFromServer = "";

        while(!messageToServer.equals("stop")){
            System.out.print("\nEnter message to server: ");
            messageToServer = buffRead.readLine();
            dout.writeUTF(messageToServer);
            dout.flush();
            messageFromServer = din.readUTF();
            System.out.println("Message from server: "+messageFromServer);
        }
        dout.close();
        socket.close();
    }
}
