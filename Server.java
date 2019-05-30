package robotstation;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import javafx.application.Application;
import javafx.concurrent.Task;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;

public class GUI extends Application {
    public static final int HEIGHT = 500;
    public static final int WIDTH = 500;
    private DataOutputStream out; //robot's output, we send messages here
    Pane pane = new Pane();
    Scene scene =  new Scene(pane, HEIGHT, WIDTH); // main window params
    
    private final int PORT = 8181;
    private final double TURN = 0.75;   // turn coefficient. decreasing it makes
                                        // robot turn more sharply
    
    private final int STEP = 10;        // change the speed when the W or S is pressed

    
    private int speed = 10;             // default speed
      
    private boolean isUpPressed = false; // vars that store state of  current presssed buttons
    private boolean isLeftPressed = false;
    private boolean isRightPressed = false;
    private boolean isDownPressed = false;
    
    private boolean isDrivingOnTheLineMode = false; // "stop" or "start" state
    
    @Override
    public void start(Stage primaryStage) { // main GUI method
        primaryStage.setTitle("Robot GUI"); 
        primaryStage.setScene(scene);
        scene.setOnKeyPressed(pressedListener);     // set listeners to handle
        scene.setOnKeyReleased(releasedListener);   // keys press/release 
        
        new Thread(robotStation).start();   // start thread to listen robot 
        primaryStage.show();
    }

    private EventHandler<KeyEvent> pressedListener = new EventHandler<KeyEvent>() {
        @Override
        public void handle(KeyEvent event) {
                switch(event.getCode()) {   // change state when some key pressed.
                                            // sending "!state" is handled by
                                            // sendMessage() method
                    case    UP :
                            if (!isUpPressed){
                                isUpPressed = true;
                                sendMessage("!state");
                            }
                            break;
                    case    DOWN:
                            if (!isDownPressed){
                                isDownPressed = true;
                                sendMessage("!state");
                            }
                            break;
                    case    LEFT:
                            if (!isLeftPressed){
                                isLeftPressed = true;
                                sendMessage("!state");
                            }
                            break;
                    case    RIGHT :
                            if (!isRightPressed){
                                isRightPressed = true;
                                sendMessage("!state");
                            }
                            break;
                    case    W :
                            if (speed < 100){
                                speed += STEP; // increase speed 
                                sendMessage("!state");
                            }
                            break;
                    case    S:
                            if (speed > 0){
                                speed -= STEP; // decrease speed
                                sendMessage("!state");
                            }
                            break;
                    case    SPACE:
                            sendMessage("M1:0,M2:0"); // forced braking
                            break;
                    case    R:
                            if (isDrivingOnTheLineMode){
                                isDrivingOnTheLineMode = false;
                                sendMessage("stop"); // disable driving on the line
                            } else {
                                isDrivingOnTheLineMode = true;
                                sendMessage("start"); // enable driving on the line
                            }
                            break;
                    case    M:
                            sendMessage("manual"); // manual mode
                            break;
                    }
                
                event.consume();
        }
    };
    
    private EventHandler<KeyEvent> releasedListener = new EventHandler<KeyEvent>() {
        @Override
        public void handle(KeyEvent event) { // handle key releasing and change
                                             // current state
            switch(event.getCode()) { 
                    case    UP :
                            if (isUpPressed){
                                isUpPressed = false;
                                sendMessage("!state"); 
                            }
                            break;
                    case    DOWN:
                            if (isDownPressed){
                                isDownPressed = false;
                                sendMessage("!state"); 
                            }
                            break;
                    case    LEFT:
                            if (isLeftPressed){
                                isLeftPressed = false;
                                sendMessage("!state");
                            }
                            break;
                    case    RIGHT :
                            if (isRightPressed){
                                isRightPressed = false;
                                sendMessage("!state");
                            }
                            break;
            }
        }
    };
    
    Task robotStation = new Task<Void>() { // kind of thread that normally use in JavaFX
        public String str = "null"; // default string from robot
        private InputStream is;
        
        @Override
        public Void call() {
            while(true){ // wait for connection to robot in the loop
                init(); 
                try (BufferedReader in = new BufferedReader(new InputStreamReader(is))){
                    while(true){
                        str = in.readLine();    // save message from robot the print it
                        System.out.println("Global: " + str); 
                    }
                } catch (Throwable ex) {
                    System.out.println("DISCONNECTED"); 
                }
            }  
        }
        
        public void init(){
            try (ServerSocket server= new ServerSocket(PORT)){ 
                System.out.println(NetworkInterface.getByName("wlan1")
                        .getInterfaceAddresses().get(0).getAddress()
                        .getHostAddress() + ":" + PORT); // print ip address
                System.out.println("TRY TO CONNECT");
                Socket client = server.accept();
                is = client.getInputStream();                           // set input
                out = new DataOutputStream(client.getOutputStream());   // set outout
                System.out.println("CONNECTED");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    };
    
    public void sendMessage(String message){
        try {
            if (message.equals("!state")){ // if get "!state" send current state params
                if (isUpPressed&&isLeftPressed&&!isRightPressed&&!isDownPressed){
                    System.out.println("M1:" + (int) (speed*TURN) + ",M2:" + speed);
                    out.writeUTF("M1:" + (int) (speed*TURN) + ",M2:" + speed);
                    out.flush();
                } else if (isUpPressed&&!isLeftPressed&&!isRightPressed&&!isDownPressed){
                    System.out.println("M1:" + speed + ",M2:" + speed);
                    out.writeUTF("M1:" + speed + ",M2:" + speed);
                    out.flush();
                } else if (!isUpPressed&&isLeftPressed&&!isRightPressed&&!isDownPressed){
                    System.out.println("M1:" + -speed + ",M2:" + speed);
                    out.writeUTF("M1:" + -speed + ",M2:" + speed);
                    out.flush();
                } else if (isUpPressed&&!isLeftPressed&&isRightPressed&&!isDownPressed){
                    System.out.println("M1:" + speed + ",M2:" + (int) (speed*TURN));
                    out.writeUTF("M1:" + speed + ",M2:" + (int) (speed*TURN));
                    out.flush();
                } else if (!isUpPressed&&!isLeftPressed&&isRightPressed&&!isDownPressed){
                    System.out.println("M1:" + speed + ",M2:" + -speed);
                    out.writeUTF("M1:" + speed + ",M2:" + -speed);
                    out.flush();
                } else if (!isUpPressed&&!isLeftPressed&&!isRightPressed&&isDownPressed){
                    System.out.println("M1:" + -speed + ",M2:" + -speed);
                    out.writeUTF("M1:" + -speed + ",M2:" + -speed);
                    out.flush();
                } else if (!isUpPressed&&!isLeftPressed&&!isRightPressed&&!isDownPressed){
                    System.out.println("M1:0,M2:0");
                    out.writeUTF("M1:0,M2:0");
                    out.flush();
                }
            } else { // otherwise send text
                System.out.println(message);
                out.writeUTF(message);
                out.flush();
            } 
        } catch (Throwable ex) {
            System.out.println("FAILED TO SEND");
        }
    }
    
    public static void main(String[] args) {
        launch(args);
    }
}
