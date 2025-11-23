import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.geometry.*;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.application.Application;
import javafx.beans.binding.BooleanBinding;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.ListView;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

import javafx.scene.web.WebView;



import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;

public class UI extends Application {



    // Annoying request nonsense. Actual GUI below

    private static final String URL =
        "https://deloreg.com/generate_AMOGUS_WILL_EAT_YOUR_SOUL";
    private static Message sendToLocalModel(ObservableList<Message> messages)
                                                        throws Exception {
        String jsonBody = buildJson(messages);
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(URL))
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.ofString(jsonBody))
                .build();
        HttpResponse<String> response =
                client.send(request, HttpResponse.BodyHandlers.ofString());
        String body = response.body();
        return new Message("assistant", body);
    }
    private static String buildJson(ObservableList<Message> messages) {
        StringBuilder sb = new StringBuilder();
        sb.append("{\"messages\":[");
        for (int i = 0; i < messages.size(); i++) {
            Message m = messages.get(i);
            if (i > 0) sb.append(',');
            sb.append("{\"role\":\"")
              .append(escape(m.role()))
              .append("\",\"content\":\"")
              .append(escape(m.content()))
              .append("\"}");
        }
        sb.append("]}");
        return sb.toString();
    }
    private static String escape(String s) {
        if (s == null) return "";
        return s
                .replace("\\", "\\\\")
                .replace("\"", "\\\"")
                .replace("\n", "\\n")
                .replace("\r", "\\r");
    }



    // Actual GUI



    // State. There is **no** state in the application other than this
    // I don't fw oop. I only code React
    // PS: I don't actually code React. I only code Svelte

    enum Screen {
        HOME,
        CONTROL
    }

    record Message(String role, String content) {}

    class State {
        // I don't fw getter or setters either
        public final ObjectProperty<Screen> currentScreen =
            new SimpleObjectProperty<>(Screen.HOME);
        public final ObservableList<Message> messages =
            FXCollections.observableArrayList();
    }



    // Views

    // A Main menu view
    private Node viewHomeCreate(State state) {

        StackPane root = new StackPane();

        // Background view
        Image gif = new Image(
                getClass().getResource("/resources/main_menu.gif").toExternalForm()
        );
        ImageView imageView = new ImageView(gif);
        imageView.setPreserveRatio(true);
        imageView.setFitWidth(1200);
        // TODO: make the image resize with the window and unlock the ability
        // to resize the window for the user
        root.getChildren().add(imageView);

        // Main view
        HBox view = new HBox();
        root.getChildren().add(view);

        // The menu buttons
        VBox menuButtons = new VBox();
        view.getChildren().add(menuButtons);

        menuButtons.getChildren().add(new Label("This is the main menu"));

        Button controlButton = new Button("Go to control");
        menuButtons.getChildren().add(controlButton);
        controlButton.setOnAction(e -> state.currentScreen.set(Screen.CONTROL));

        return root;
    }

    // A view for the controller view view
    private Node viewControlCreate(State state) {

        StackPane root = new StackPane();

        // Background view
        // TODO

        // Main view
        HBox view = new HBox();
        root.getChildren().add(view);

        // The control pannel
        VBox controlPanel = new VBox();
        view.getChildren().add(controlPanel);
        Button aboutButton = new Button("Go to main menu");
        aboutButton.setOnAction(e -> state.currentScreen.set(Screen.HOME));
        controlPanel.getChildren().add(new Label("This is control view"));
        controlPanel.getChildren().add(aboutButton);

        // The main map
        // TODO: Add the map

        // The control pannel
        VBox mainMap = new VBox();
        view.getChildren().add(mainMap);
        // TODO: Add the actual buttons for controlling the clanker

        // NOTE: The AI chat application will be removed in future releases
        // due to the fact there's no connection when controlling the clanker
        // The AI chat application
        VBox chat = new VBox();
        view.getChildren().add(chat);

        // The AI chat application - Title
        Label chatTitle = new Label("Chat with our AI assistant - Emi");
        chat.getChildren().add(chatTitle);

        // The AI chat application - Messages
        ListView<Message> listView = new ListView<>();
        chat.getChildren().add(listView);
        listView.setItems(state.messages);
        listView.setCellFactory(lv -> new ListCell<>() {
            @Override
            protected void updateItem(Message msg, boolean empty) {
                super.updateItem(msg, empty);
                if (empty || msg == null || msg.role().equals("system")) {
                    setText(null);
                    setGraphic(null);
                } else {
                    setText(msg.content());
                    if (msg.role().equals("user")) {
                        // TODO: style for the user
                    } else {
                        // TODO: style for the assistant
                    }
                }
            }
        });

        // The AI chat application - Your message field
        HBox sendField = new HBox();
        chat.getChildren().add(sendField);

        TextField inputField = new TextField();
        sendField.getChildren().add(inputField);

        Button sendButton = new Button("Send");
        sendField.getChildren().add(sendButton);
        sendButton.setOnAction(e -> {
            String text = inputField.getText().trim();
            if (text.isEmpty()) return;

            state.messages.add(new Message("user", text));
            inputField.clear();

            // Scroll to bottom when new message added
            listView.scrollTo(state.messages.size() - 1);

            try {
                Message reply = sendToLocalModel(state.messages);
                state.messages.add(reply);
            } catch (Exception g) {
                return;
            }

            // Scroll to bottom when new message added
            listView.scrollTo(state.messages.size() - 1);

            
        });
        inputField.setOnAction(sendButton.getOnAction());

        return root;
    }

    private void bindVisibilityToScreen(Node node, State state, Screen screen) {
        BooleanBinding isActive =
                state.currentScreen.isEqualTo(screen);

        node.visibleProperty().bind(isActive);
        node.managedProperty().bind(isActive);
    }

    @Override
    public void start(Stage stage) {

        
        State state = new State();
        state.messages.add(new Message("system", "You are a silly little "
            + "uwu AI assistant that is at the same time cute and egotistical. "
            + "You commnicate with incomprehensible cute yet offensive slang like "
            + "you keep yeeting at them noobs hehe~ :D You keep your responses "
            + "short, casual and not very informative"));

        // Static view creation. The objects will be preserved for the entire
        // duration of the program
        Node viewHome = viewHomeCreate(state);
        Node viewControl = viewControlCreate(state);

        // Root node creation
        StackPane content = new StackPane(viewHome, viewControl);
        BorderPane root = new BorderPane(content);

        bindVisibilityToScreen(viewHome, state, Screen.HOME);
        bindVisibilityToScreen(viewControl, state, Screen.CONTROL);

        Scene scene = new Scene(root, 1200, 675);
        stage.setTitle("Fully reactive app");
        stage.setScene(scene);
        stage.setResizable(false);
        stage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
