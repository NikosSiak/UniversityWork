package app;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class FrameMenu extends JMenuBar {

    private JMenu fileMenu, editMenu;
    private JMenuItem fileNew, fileOpen, fileSave, fileCopy, fileExit;
    private JMenuItem editClear, editStatistics;

    TextEditorListener listener;


    public FrameMenu() {
        listener = new TextEditorListener();

        fileMenu = new JMenu("File");
        fileNew = new JMenuItem("New");
        fileOpen = new JMenuItem("Open");
        fileSave = new JMenuItem("Save");
        fileCopy = new JMenuItem("Save as");
        fileExit = new JMenuItem("Exit");

        fileNew.setActionCommand("New");
        fileOpen.setActionCommand("Open");
        fileSave.setActionCommand("Save");
        fileCopy.setActionCommand("SaveAs");
        fileExit.setActionCommand("Exit");

        fileNew.addActionListener(listener);
        fileOpen.addActionListener(listener);
        fileSave.addActionListener(listener);
        fileCopy.addActionListener(listener);
        fileExit.addActionListener(listener);

        fileMenu.add(fileNew);
        fileMenu.add(fileOpen);
        fileMenu.add(fileSave);
        fileMenu.add(fileCopy);
        fileMenu.add(fileExit);

        editMenu = new JMenu("Edit");
        editClear = new JMenuItem("Clear");
        editStatistics = new JMenuItem("Statistics");

        editClear.setActionCommand("Clear");
        editStatistics.setActionCommand("Statistics");

        editClear.addActionListener(listener);
        editStatistics.addActionListener(listener);

        editMenu.add(editClear);
        editMenu.add(editStatistics);

        this.add(fileMenu);
        this.add(editMenu);
    }
    
    
}