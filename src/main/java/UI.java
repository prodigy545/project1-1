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

import javafx.beans.binding.Bindings;
import javafx.beans.binding.BooleanBinding;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;

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



    // State. There is **no** state in the application other than this
    // I don't fw oop. I only code React
    // PS: I don't actually code React. I only code Svelte

    enum Screen {
        HOME,
        CONTROL
    }

    enum Transmission {
        AUTOMATIC,
        MANUAL
    }

    class State {
        // I don't fw getter or setters either
        public final ObjectProperty<Transmission> transmission =
            new SimpleObjectProperty<>(Transmission.AUTOMATIC);
        public final ObjectProperty<Screen> currentScreen =
            new SimpleObjectProperty<>(Screen.HOME);
    }



    // Views

    // A Main menu view
    private Node viewHomeCreate(State state) {

        StackPane root = new StackPane();

        // Background view
        Image gif = new Image(
                getClass().getResource("/main_menu.gif").toExternalForm()
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


        
        // Transmission selection
        Label automaticLabel = new Label("Automatic transmission selected");
        Label manualLabel = new Label("Manual transmission selected");
        StackPane transmissionSelectionStack = new StackPane(
            automaticLabel,
            manualLabel
        );
        controlPanel.getChildren().add(transmissionSelectionStack);
        BooleanBinding isAutomatic =
                state.transmission.isEqualTo(Transmission.AUTOMATIC);
        automaticLabel.visibleProperty().bind(isAutomatic);
        automaticLabel.managedProperty().bind(isAutomatic);
        BooleanBinding isManual =
                state.transmission.isEqualTo(Transmission.MANUAL);
        manualLabel.visibleProperty().bind(isManual);
        manualLabel.managedProperty().bind(isManual);
        Button switchTransmission = new Button();
        switchTransmission.textProperty().bind(
            Bindings.when(isAutomatic)
                    .then("Chose manual")
                    .otherwise("Chose automatic")
        );
        switchTransmission.setOnAction(e -> {
            if (state.transmission.isEqualTo(Transmission.MANUAL).get()) {
                state.transmission.set(Transmission.AUTOMATIC);
            } else {
                state.transmission.set(Transmission.MANUAL);
            }
        });
        controlPanel.getChildren().add(switchTransmission);

        
        // Arrow keys
        GridPane arrowGrid = new GridPane();
        controlPanel.getChildren().add(arrowGrid);

        Button upButton = new Button("UP");
        arrowGrid.add(upButton, 1, 0);

        Button leftButton = new Button("LEFT");
        arrowGrid.add(leftButton, 0, 1);

        Button rightButton = new Button("RIGHT");
        arrowGrid.add(rightButton, 2, 1);

        Button downButton = new Button("DOWN");
        arrowGrid.add(downButton, 1, 2);

        // The main map
        VBox mainMap = new VBox();
        view.getChildren().add(mainMap);
        // TODO: Add the map

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
