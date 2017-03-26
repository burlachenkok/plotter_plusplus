#ifndef PLOTTER_IMAGE_H
#define PLOTTER_IMAGE_H

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QListWidgetItem>

namespace Ui {
    class plotter_image;
}

class plotter_image : public QWidget
{
    Q_OBJECT

public:
    explicit plotter_image(QWidget *parent = 0);
    ~plotter_image();

    virtual void resizeEvent(QResizeEvent* event);

    void updateViewerImage();

public slots:
    void slotHandleBitmapInSpecifiedFormat(const void* bitmap, int width, int height, int sizeInBytes);
    void on_btnSaveImage_clicked();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void on_spbImageScale_valueChanged(double arg1);

    void on_spbImageScale_editingFinished();

    void on_cbxIncomeOrder_activated(int /*index*/);

    void on_cbxRed_clicked();

    void on_cbxGreen_clicked();

    void on_cbxBlue_clicked();

    void on_sliderScale_valueChanged(int value);

    void on_lstWidgetItems_itemDoubleClicked(QListWidgetItem *item);

    void on_btnLoadImage_clicked();

    void on_btnCleanAllHistory_clicked();

private:
    Ui::plotter_image *ui;
    QImage lastReceivedImage;
    unsigned int imageIndex;
    QMap<QString, QImage> imageHistory;
};

#endif // PLOTTER_IMAGE_H
