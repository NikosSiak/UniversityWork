package app;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;

import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.text.BadLocationException;

public class TextEditorListener implements ActionListener {

    private JFileChooser fileChooser;
    private static String filePath;
    private static boolean openedFile = false;

    public TextEditorListener() {
        fileChooser = new JFileChooser();
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getActionCommand().equals("Open")){
            openFile();
        }
        else if (e.getActionCommand().equals("Save")){
            saveFile();
        }
        else if (e.getActionCommand().equals("SaveAs")){
            openedFile = false;
            saveFile();
        }
        else if (e.getActionCommand().equals("Clear")){
            TextEditor.fileContent.setText("");
        }
        else if (e.getActionCommand().equals("New")){
            if (JOptionPane.showConfirmDialog(TextEditor.fileContent, "Any unsaved data will be lost, are you sure?",
                    "Start new file", JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION) {
                filePath = "";
                TextEditor.filePathText.setText("");
                TextEditor.fileContent.setText("");
                openedFile = false;
            }
        }
        else if (e.getActionCommand().equals("Statistics")){
            showStatistics();
        }
        else if (e.getActionCommand().equals("Exit")){
            if (JOptionPane.showConfirmDialog(TextEditor.fileContent, "Are you sure you want to close this window?",
                    "Close Window?", JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION) {
                System.exit(0);
            }
        }
    }

    private void openFile() {
        if (fileChooser.showOpenDialog(TextEditor.fileContent) == JFileChooser.APPROVE_OPTION) {
            filePath = fileChooser.getSelectedFile().getAbsolutePath();
            TextEditor.fileContent.setText("");
            try {
                BufferedReader reader = new BufferedReader(
                        new InputStreamReader(new FileInputStream(filePath), "UTF8"));
                String line = reader.readLine();
                while (line != null) {
                    TextEditor.fileContent.append(line + "\n");
                    line = reader.readLine();
                }
                reader.close();
                TextEditor.filePathText.setText(filePath);
                openedFile = true;
            } catch (IOException e1) {
                TextEditor.filePathText.setForeground(new Color(255, 0, 0));
                TextEditor.filePathText.setText("Can't open file");
                TextEditor.fileContent.setText("");
                openedFile = false;
            }
        }
    }

    private void saveFile() {
        if (openedFile) {
            filePath = TextEditor.filePathText.getText();
        } else {
            int retval = fileChooser.showSaveDialog(TextEditor.fileContent);
            if (retval == JFileChooser.APPROVE_OPTION) {
                filePath = fileChooser.getSelectedFile().getAbsolutePath();
            } else {
                return;
            }
        }
        try {
            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(filePath), "UTF8"));
            writer.write(TextEditor.fileContent.getText());
            writer.flush();
            writer.close();
            TextEditor.filePathText.setText(filePath);
            openedFile = true;
        } catch (UnsupportedEncodingException e1) {
            JOptionPane.showMessageDialog(TextEditor.fileContent, "Unsupported encoding", "Error", JOptionPane.ERROR_MESSAGE);
        } catch (FileNotFoundException e1) {
            e1.printStackTrace();
            JOptionPane.showMessageDialog(TextEditor.fileContent, "File not Found!", "Error", JOptionPane.ERROR_MESSAGE);
        } catch (IOException e1) {
            e1.printStackTrace();
            JOptionPane.showMessageDialog(TextEditor.fileContent, "Something Happened while writing the file", "Error",
                    JOptionPane.ERROR_MESSAGE);
        }
    }

    private void showStatistics() {
        String[] message = new String[5];
        String[] words = TextEditor.fileContent.getText().split("[\n ]");
        int charCount = 0;
        int numberOfWords = 0;
        int paragraphs = 0;
        boolean newParagraph = true;

        for (String word : words) {
            charCount += word.length();
            if (!word.isEmpty()) {
                numberOfWords++;
            }
        }

        for (int i = 0; i < TextEditor.fileContent.getLineCount(); i++) {
            try {
                if (!TextEditor.fileContent.getText(TextEditor.fileContent.getLineStartOffset(i), 1).equals("\n") && newParagraph) {
                    paragraphs++;
                    newParagraph = false;
                } else if (TextEditor.fileContent.getText(TextEditor.fileContent.getLineStartOffset(i), 1).equals("\n")) {
                    newParagraph = true;
                }
            } catch (BadLocationException e) {
                e.printStackTrace();
            }
        }

        saveFile();

        message[0] = "Number of words: " + numberOfWords;
        message[1] = "Number of characters: " + charCount;
        message[2] = "Number of characters including spaces and new lines: " + (charCount + words.length - 1);
        message[3] = "Number of paragraphs: " + paragraphs;
        message[4] = "Size of file: " + (new File(filePath).length()) + " bytes";

        JOptionPane.showMessageDialog(TextEditor.fileContent, message, "Statistics", JOptionPane.INFORMATION_MESSAGE);
    }
}