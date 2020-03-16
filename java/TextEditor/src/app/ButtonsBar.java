package app;

import java.awt.GridLayout;

import javax.swing.JButton;
import javax.swing.JPanel;

public class ButtonsBar extends JPanel {

    private JButton openBtn;
    private JButton saveBtn;
    private JButton copyBtn;
    private JButton clearBtn;
    private JButton newBtn;
    private JButton statisticsBtn;
    private JButton exitBtn;

    TextEditorListener listener;


    public ButtonsBar() {
        this.setLayout(new GridLayout(1, 0));
        listener = new TextEditorListener();

        openBtn = new JButton("Open");
        saveBtn = new JButton("Save");
        copyBtn = new JButton("Save as");
        clearBtn = new JButton("Clear");
        newBtn = new JButton("New");
        statisticsBtn = new JButton("Statistics");
        exitBtn = new JButton("Exit");

        openBtn.setActionCommand("Open");
        saveBtn.setActionCommand("Save");
        copyBtn.setActionCommand("SaveAs");
        clearBtn.setActionCommand("Clear");
        newBtn.setActionCommand("New");
        statisticsBtn.setActionCommand("Statistics");
        exitBtn.setActionCommand("Exit");

        openBtn.addActionListener(listener);
        saveBtn.addActionListener(listener);
        copyBtn.addActionListener(listener);
        clearBtn.addActionListener(listener);
        newBtn.addActionListener(listener);
        statisticsBtn.addActionListener(listener);
        exitBtn.addActionListener(listener);

        this.add(newBtn);
        this.add(openBtn);
        this.add(saveBtn);
        this.add(copyBtn);
        this.add(clearBtn);
        this.add(statisticsBtn);
        this.add(exitBtn);
    }
    
}