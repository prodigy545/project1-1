    import java.net.URI;
    import java.net.http.HttpClient;
    import java.net.http.HttpRequest;
    import java.net.http.HttpResponse;
    import java.time.Duration;
    import java.util.Scanner;

    public class ArduinoClient {
        public static void main(String[] args) throws Exception {
            Scanner sc = new Scanner(System.in);
            System.out.println("Select an option: Enter a command: f = forward, b = backwards, l = move left, r = move right, cc = rotate counter clockwise (CCW), c = rotate clockwise (CW), sl = move sideways left, sr = move sideways right , s = stop all motors, ss = set speed (e.g., s200 for speed = 200),autolineon = auto line following mode on,autolineoff = autoline following mode off  q = quit");
            while (true) {
                System.out.println("Enter a command : ");
                String inp = sc.nextLine();

                if (inp.equals("q")){
                    System.out.println("Exiting program. :)");
                    break;
                }

                if(inp.equals(" ") || inp.equals("")){
                        System.out.println("Wrong command. ");
                        continue;
                }
                
                if (inp.equals("f")) {

                    String url = "http://192.168.1.1/forward";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());

            }else if (inp.equals("s")) {

                    String url = "http://192.168.1.1/stop";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("b")) {

                    String url = "http://192.168.1.1/backwards";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("l")) {

                    String url = "http://192.168.1.1/left";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("r")) {

                    String url = "http://192.168.1.1/right";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("c")) {

                    String url = "http://192.168.1.1/clockwise";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("cc")) {

                    String url = "http://192.168.1.1/counterclockwise";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            }else if (inp.equals("ss")) {
                    int newValue =0;
                    while(true){
                    System.out.println("Give a speed value valid from 1-255:");
                    newValue = sc.nextInt();
                    sc.nextLine();
                    if (newValue >=1 && newValue<=255){
                        break;
                    }else{
                        System.out.println("Please give a valid speed");
                    }
                    }
                    String url = "http://192.168.1.1/setspeed?value=" + newValue;

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            
            } else if(inp.equals("sl")){
                    String url = "http://192.168.1.1/movesidewaysleft";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            } else if(inp.equals("sr")){
                    String url = "http://192.168.1.1/movesidewaysright";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            }else if(inp.equals("autolineon")){
                    String url = "http://192.168.1.1/autolineon";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            }else if(inp.equals("autolineff")){
                    String url = "http://192.168.1.1/autolineoff";

                    HttpClient client = HttpClient.newBuilder()
                            .connectTimeout(Duration.ofSeconds(5))
                            .build();

                    HttpRequest req = HttpRequest.newBuilder()
                            .uri(URI.create(url))
                            .timeout(Duration.ofSeconds(10))
                            .GET()
                            .build();

                    HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());
                            System.out.println("Body: " + res.body());
            }
          
          }

         }

      }

    
