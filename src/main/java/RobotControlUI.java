import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.geometry.*;
import javafx.stage.Stage;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

public class RobotControlUI extends Application {

    private static final String BASE_URL = "http://192.168.1.1";
    private final HttpClient client = HttpClient.newBuilder()
            .connectTimeout(Duration.ofSeconds(5))
            .build();

    // Robot state
    private int currentSpeed = 90;
    private int obstacleDistance = 100; // Distance to obstacle in cm
    private boolean emergencyStopActive = false;

    // UI Components
    private Label modeToggleLabel;
    private ListView<String> historyList;
    private ObservableList<String> historyItems;
    private Label statusLabel;
    private Label speedLabel;
    private Label modeLabel;
    private Label distanceLabel;
    private Label emergencyLabel;

    // State tracking
    private boolean isAutoLineMode = false;

    @Override
    public void start(Stage stage) {
        historyItems = FXCollections.observableArrayList();

        BorderPane root = new BorderPane();
        root.setPadding(new Insets(10));

        // Top - Status bar
        root.setTop(createStatusBar());

        // Left - Control Panel
        root.setLeft(createControlPanel());

        // Right - History panel
        root.setRight(createHistoryPanel());

        Scene scene = new Scene(root, 1400, 800);
        stage.setTitle("Robot Control ");
        stage.setScene(scene);
        stage.show();

    

        // Start monitoring distance and emergency stop
        startDistanceMonitoring();
    }

    private void startDistanceMonitoring() {
        javafx.animation.Timeline distanceMonitor = new javafx.animation.Timeline(
                new javafx.animation.KeyFrame(javafx.util.Duration.millis(200), e -> {
                    if (!emergencyStopActive) {
                        obstacleDistance = 30 + (int) (Math.random() * 100);
                    }

                    distanceLabel.setText("Distance: " + obstacleDistance + " cm");
                }));
        distanceMonitor.setCycleCount(javafx.animation.Timeline.INDEFINITE);
        distanceMonitor.play();
    }

    private HBox createStatusBar() {
        HBox statusBar = new HBox(20);
        statusBar.setPadding(new Insets(10));

        statusLabel = new Label("Status: Ready");
        speedLabel = new Label("Speed: " + currentSpeed);
        modeLabel = new Label("Mode: Manual");
        distanceLabel = new Label("Distance: " + obstacleDistance + " cm");
        emergencyLabel = new Label("Emergency Stop: OFF");

        statusBar.getChildren().addAll(statusLabel, speedLabel, modeLabel, distanceLabel,emergencyLabel);
        return statusBar;
    }

    private VBox createControlPanel() {
        VBox controlPanel = new VBox(15);
        controlPanel.setPadding(new Insets(10));
        controlPanel.setPrefWidth(280);

        Label title = new Label("Robot Controls");
        title.setStyle("-fx-font-size: 18px; -fx-font-weight: bold;");

        VBox modeToggleSection = createModeToggleSection();
        VBox basicMovement = createMovementSection();
        VBox rotationSection = createRotationSection();
        VBox sidewaysSection = createSidewaysSection();
        VBox speedSection = createSpeedSection();
        VBox autoLineSection = createAutoLineSection();


controlPanel.getChildren().addAll(title, new Separator(), modeToggleSection,
                new Separator(), basicMovement,
                new Separator(), rotationSection,
                new Separator(), sidewaysSection,
                new Separator(), speedSection,
                new Separator(), autoLineSection);

        ScrollPane scrollPane = new ScrollPane(controlPanel);
        scrollPane.setFitToWidth(true);

        VBox wrapper = new VBox(scrollPane);
        return wrapper;

        };

       
    

    private VBox createModeToggleSection() {
        VBox section = new VBox(10);
        Label label = new Label("Operation Mode");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        modeToggleLabel= new Label((isAutoLineMode) ? "Automatic Mode" : "Manual Mode");
        modeToggleLabel.setPrefWidth(250);
        modeToggleLabel.setPrefHeight(40);

        updateModeToggleButton();

        section.getChildren().addAll(label, modeToggleLabel);
        return section;

        };
    
    


    private VBox createMovementSection() {
        VBox section = new VBox(10);
        Label label = new Label("Basic Movement");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        GridPane grid = new GridPane();
        grid.setHgap(5);
        grid.setVgap(5);
        grid.setAlignment(Pos.CENTER);

        Button forward = createControlButton("↑", "Forward");
        forward.setOnAction(e -> {
            sendCommand("/forward", "Move Forward");
        });

        Button backward = createControlButton("↓", "Backward");
        backward.setOnAction(e -> {
            sendCommand("/backwards", "Move Backward");
        });

        Button left = createControlButton("←", "Left");
        left.setOnAction(e -> {
            sendCommand("/left", "Turn Left");

        });

        Button right = createControlButton("→", "Right");
        right.setOnAction(e -> {
            sendCommand("/right", "Turn Right");

        });

        Button stop = createControlButton("■", "Stop");
        stop.setStyle("-fx-background-color: #e74c3c; -fx-font-size: 18px;");
        stop.setOnAction(e -> {
            sendCommand("/stop", "Stop");
            isAutoLineMode = false;
            updateModeLabel();
            updateModeToggleButton();
        });

        grid.add(forward, 1, 0);
        grid.add(left, 0, 1);
        grid.add(stop, 1, 1);
        grid.add(right, 2, 1);
        grid.add(backward, 1, 2);

        section.getChildren().addAll(label, grid);
        return section;
    }

    private VBox createRotationSection() {
        VBox section = new VBox(10);
        Label label = new Label("Rotation");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        HBox buttons = new HBox(10);
        buttons.setAlignment(Pos.CENTER);

        Button clockwise = createControlButton("↻", "Clockwise");
        clockwise.setOnAction(e -> {
            sendCommand("/clockwise", "Rotate Clockwise");

        });

        Button counterClockwise = createControlButton("↺", "Counter-Clockwise");
        counterClockwise.setOnAction(e -> {
            sendCommand("/counterclockwise", "Rotate Counter-Clockwise");

        });

        buttons.getChildren().addAll(counterClockwise, clockwise);
        section.getChildren().addAll(label, buttons);
        return section;
    }

    private VBox createSidewaysSection() {
        VBox section = new VBox(10);
        Label label = new Label("Sideways Movement");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        HBox buttons = new HBox(10);
        buttons.setAlignment(Pos.CENTER);

        Button sidewaysLeft = createControlButton("⇐", "Sideways Left");
        sidewaysLeft.setOnAction(e -> {
            sendCommand("/movesidewaysleft", "Move Sideways Left");
        });

        Button sidewaysRight = createControlButton("⇒", "Sideways Right");
        sidewaysRight.setOnAction(e -> {
            sendCommand("/movesidewaysright", "Move Sideways Right");
        });

        buttons.getChildren().addAll(sidewaysLeft, sidewaysRight);
        section.getChildren().addAll(label, buttons);
        return section;
    }

    private VBox createSpeedSection() {
        VBox section = new VBox(10);
        Label label = new Label("Speed Control");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        Slider speedSlider = new Slider(0, 255, currentSpeed);
        speedSlider.setShowTickLabels(true);
        speedSlider.setShowTickMarks(true);
        speedSlider.setMajorTickUnit(50);
        speedSlider.setBlockIncrement(10);

        Label sliderValue = new Label("Speed: " + (int) speedSlider.getValue());

        speedSlider.valueProperty().addListener((obs, oldVal, newVal) -> {
            sliderValue.setText("Speed: " + newVal.intValue());
        });

        Button setSpeed = new Button("Set Speed");
        setSpeed.setStyle("-fx-background-color: #3498db; -fx-text-fill: white;");
        setSpeed.setOnAction(e -> {
            int speed = (int) speedSlider.getValue();
            sendCommand("/setspeed?value=" + speed, "Set Speed to " + speed);
            currentSpeed = speed;
            speedLabel.setText("Speed: " + speed);
        });

        section.getChildren().addAll(label, speedSlider, sliderValue, setSpeed);
        return section;
    }

    private VBox createAutoLineSection() {
        VBox section = new VBox(10);
        Label label = new Label("Auto Line Following");
        label.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

        HBox buttons = new HBox(10);
        buttons.setAlignment(Pos.CENTER);

        Button autoLineOn = new Button("Start Auto Line");
        autoLineOn.setStyle("-fx-background-color: #27ae60; -fx-text-fill: white;");
        autoLineOn.setOnAction(e -> {
            sendCommand("/autolineon", "Auto Line Following ON");
            isAutoLineMode = true;
            updateModeLabel();
            updateModeToggleButton();
        });//b

        Button autoLineOff = new Button("Stop Auto Line");
        autoLineOff.setStyle("-fx-background-color: #ff0000ff; -fx-text-fill: white;");
        autoLineOff.setOnAction(e -> {
            sendCommand("/autolineoff", "Auto Line Following OFF");
            isAutoLineMode = false;
            updateModeLabel();
            updateModeToggleButton();
        });

        buttons.getChildren().addAll(autoLineOn, autoLineOff);
        section.getChildren().addAll(label, buttons);
        return section;
    }

    private Button createControlButton(String symbol, String tooltip) {
        Button btn = new Button(symbol);
        btn.setPrefSize(70, 70);
        btn.setStyle("-fx-background-color: #3498db; -fx-text-fill: white; " +
                "-fx-font-size: 24px; -fx-font-weight: bold;");
        btn.setTooltip(new Tooltip(tooltip));

        btn.setOnMouseEntered(e -> btn.setStyle("-fx-background-color: #2980b9; -fx-text-fill: white; " +
                "-fx-font-size: 24px; -fx-font-weight: bold;"));
        btn.setOnMouseExited(e -> btn.setStyle("-fx-background-color: #3498db; -fx-text-fill: white; " +
                "-fx-font-size: 24px; -fx-font-weight: bold;"));

        return btn;
    }

    private VBox createHistoryPanel() {
        VBox historyPanel = new VBox(10);
        historyPanel.setPadding(new Insets(10));
        historyPanel.setPrefWidth(300);

        Label title = new Label("Action History");
        title.setStyle("-fx-font-size: 18px; -fx-font-weight: bold;");

        historyList = new ListView<>(historyItems);
        historyList.setPrefHeight(650);

        Button clearHistory = new Button("Clear History");
        clearHistory.setStyle("-fx-background-color: #95a5a6; -fx-text-fill: white;");
        clearHistory.setOnAction(e -> historyItems.clear());

        historyPanel.getChildren().addAll(title, historyList, clearHistory);
        return historyPanel;
    }



    private void sendCommand(String endpoint, String action) {


        new Thread(() -> {
            try {
                String url = BASE_URL + endpoint;
                HttpRequest req = HttpRequest.newBuilder()
                        .uri(URI.create(url))
                        .timeout(Duration.ofSeconds(10))
                        .GET()
                        .build();

                HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());

                String timestamp = LocalTime.now().format(DateTimeFormatter.ofPattern("HH:mm:ss"));
                Platform.runLater(() -> {
                    statusLabel.setText("Status: " + action + " - Success");
                    addToHistory("[" + timestamp + "] " + action + " ✓");
                });

            } catch (Exception e) {
                Platform.runLater(() -> {
                    statusLabel.setText("Status: Command Failed!");
                    addToHistory("[ERROR] " + action + " failed: " + e.getMessage());
                });
            }
        }).start();
    }

    private void addToHistory(String action) {
        historyItems.add(0, action);
        if (historyItems.size() > 100) {
            historyItems.remove(100);
        }
    }

    private void updateModeLabel() {
        if (isAutoLineMode) {
            modeLabel.setText("Mode: Auto Line Following");
        } else {
            modeLabel.setText("Mode: Manual");
        }
    }

    private void updateModeToggleButton() {
         if (modeToggleLabel != null) {
            if (isAutoLineMode) {
             modeToggleLabel.setText("Automatic Mode");
             modeToggleLabel.setStyle("-fx-background-color: #27ae60; -fx-text-fill: white; " +
                    "-fx-font-size: 14px; -fx-font-weight: bold;");  
            }else {
            modeToggleLabel.setText("Manual Mode");
            modeToggleLabel.setStyle("-fx-background-color: #e74c3c; -fx-text-fill: white; " +
                    "-fx-font-size: 14px; -fx-font-weight: bold;");
        
            }
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}