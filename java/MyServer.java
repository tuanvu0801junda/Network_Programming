import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class MyServer {
    public static void main(String[] args) throws Exception{
        ServerSocket serverSocket = new ServerSocket(3333);
        Socket socket = serverSocket.accept(); //return a Socket Object
        DataInputStream din = new DataInputStream(socket.getInputStream());
        DataOutputStream dout = new DataOutputStream(socket.getOutputStream());
        BufferedReader buffRead = new BufferedReader(new InputStreamReader(System.in));

        String messageToClient = "", messageFromClient = "";
        while(!messageFromClient.equals("stop")){
            messageFromClient = din.readUTF();
            System.out.println("\nMessage From Client: "+messageFromClient);
            System.out.print("Enter your reply: ");
            messageToClient = buffRead.readLine();
            dout.writeUTF(messageToClient);
            dout.flush();
        }
        din.close();
        socket.close();
        serverSocket.close();
    }
}
