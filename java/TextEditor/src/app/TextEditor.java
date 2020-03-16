package app;

import java.awt.BorderLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class TextEditor extends JFrame {
    
    private FrameMenu menuBar;
    private ButtonsBar buttons;

    static JTextArea fileContent;
    static JTextField filePathText;

    public TextEditor() {
        super("Text Editor");

        menuBar = new FrameMenu();
        this.setJMenuBar(menuBar);

        buttons = new ButtonsBar();
        this.add(buttons, BorderLayout.NORTH);

        JPanel texts = new JPanel(new BorderLayout());
        
        filePathText = new JTextField();
        filePathText.setEditable(false);
        texts.add(filePathText, BorderLayout.NORTH);
        
        fileContent = new JTextArea();
        JScrollPane scroll = new JScrollPane(fileContent, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        texts.add(scroll);

        this.add(texts, BorderLayout.CENTER);

        this.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                if (JOptionPane.showConfirmDialog(TextEditor.this, "Are you sure you want to close this window?",
                        "Close Window?", JOptionPane.YES_NO_OPTION,
                        JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION) {
                    System.exit(0);
                }
            }
        });
        
        this.setSize(700, 500);
        this.setLocationRelativeTo(null);
        this.setVisible(true);
    }
}