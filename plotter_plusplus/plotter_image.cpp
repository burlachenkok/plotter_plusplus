#include "plotter_image.h"
#include "ui_plotter_image.h"

#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QKeyEvent>

#include <assert.h>

namespace
{
    unsigned char normalizedFloatToByte(float v) {
        if (v < 0.0f)
            return 0;
        else if (v >= 1.0f)
            return 255;
        else
            return (unsigned char)(v * 255.0f);
    }
    unsigned char normalizedDoubleToByte(double v) {
        if (v < 0)
            return 0;
        else if (v >= 1.0)
            return 255;
        else
            return (unsigned char)(v * 255.0);
    }

}
plotter_image::plotter_image(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter_image),
    imageIndex(0)
{
    ui->setupUi(this);
    ui->edtImageSizeInViewer->setText(QString("%1 x %2").arg(ui->lblViewImage->width()).arg(ui->lblViewImage->height()));

    ui->lstWidgetItems->installEventFilter(this);
}

bool plotter_image::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->lstWidgetItems && event->type() == QEvent::KeyPress)
    {
        switch ( ((QKeyEvent*)event)->key())
        {
            case Qt::Key_Delete:
            {
                auto selectedItems = ui->lstWidgetItems->selectedItems();
                for (int i = 0; i < selectedItems.size(); ++i)
                {
                    imageHistory.remove(selectedItems[i]->text());
                    delete selectedItems[i];
                }
            }
            break;
        }
    }
    return QWidget::eventFilter(object, event);
}
plotter_image::~plotter_image()
{
    delete ui;
}

void plotter_image::on_btnSaveImage_clicked()
{
    if (lastReceivedImage.isNull())
        return;

    QString imagePath = QFileDialog::getSaveFileName(this, tr("Save image dialog"), "", tr("PNG (*.png);;JPEG (*.jpg *.jpeg)" ));
    if (imagePath.isEmpty())
        return;

    QImage img = lastReceivedImage;
    if (ui->cbxIncomeOrder->currentIndex() == 0)
        img = img.transformed(QTransform().scale(1,-1).translate(0, -img.height()));
    img.save(imagePath);
}

void plotter_image::slotHandleBitmapInSpecifiedFormat(const void* bitmap,
                                                        int width,
                                                        int height,
                                                        int sizeInBytes)
{
    QImage img;
    int bpp =sizeInBytes / (width * height);
    int formatIndex = ui->cbxFormat->currentIndex();
    int size = width*height;
    unsigned char* tempBitmap = nullptr;
    //------------------------------------------------------------------------------------------//
    if (formatIndex == 0)
    {
        // RGB_U8U8U8        ([0,255], [0,255], [0,255])
        if (bpp != 3)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        img = QImage((const uchar*)bitmap, width, height, width*bpp, QImage::Format_RGB888);
    }
    else if (formatIndex == 1)
    {
        // RGB_U8U8U8U8   ([0,255], [0,255],[0,255], 255)
        if (bpp != 4)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        img = QImage((const uchar*)bitmap, width, height, width*bpp, QImage::Format_RGB32);
    }
    else if (formatIndex == 2)
    {
        // RGB_F32F32F32    ([0.0f, 1.0f], [0.0f, 1.0f], [0.0f, 1.0f])
        if (bpp != 12)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        tempBitmap = new unsigned char[width*height*3];
        for (int i = 0; i < size; ++i)
        {
            tempBitmap[3*i + 0] = normalizedFloatToByte(((float*)bitmap)[3*i + 0]);
            tempBitmap[3*i + 1] = normalizedFloatToByte(((float*)bitmap)[3*i + 1]);
            tempBitmap[3*i + 2] = normalizedFloatToByte(((float*)bitmap)[3*i + 2]);
        }
        img = QImage((const uchar*)tempBitmap, width, height, width*3, QImage::Format_RGB888);
    }
    else if (formatIndex == 3)
    {
        // GRAY_U8              ([0,255])
        if (bpp != 1)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        img = QImage((const uchar*)bitmap, width, height, width*bpp, QImage::Format_Grayscale8);
    }
    else if (formatIndex == 4)
    {
        // GRAY_F32             ([0.0f, 1.0f])
        if (bpp != 4)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        tempBitmap = new unsigned char[width*height*1];
        for (int i = 0; i < size; ++i)
            tempBitmap[i] = normalizedFloatToByte(((float*)bitmap)[i]);
        img = QImage((const uchar*)tempBitmap, width, height, width * 1, QImage::Format_Grayscale8);
    }

    else if (formatIndex == 5)
    {
        // GRAY_F32_UNNORM          ([0.0f, 255.0f])
        if (bpp != 4)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        int size = width*height;
        unsigned char* tempBitmap = new unsigned char[width*height*1];
        for (int i = 0; i < size; ++i)
            tempBitmap[i] = (unsigned char)(((float*)bitmap)[i]);
        img = QImage((const uchar*)tempBitmap, width, height, width*1, QImage::Format_Grayscale8);
    }
    else if (formatIndex == 6)
    {
        // GRAY_F64                        ([0.0, 1.0])
        if (bpp != 8)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        tempBitmap = new unsigned char[width*height*1];
        for (int i = 0; i < size; ++i)
            tempBitmap[i] = normalizedDoubleToByte(((double*)bitmap)[i]);
        img = QImage((const uchar*)tempBitmap, width, height, width*1, QImage::Format_Grayscale8);
    }
    else if (formatIndex == 7)
    {
        // GRAY_F64_UNNORM     ([0.0, 255.0])
        if (bpp != 8)
        {
            qDebug() << "Error income format! Client transmit " << bpp << " bytes per pixel\n";
            return;
        }
        unsigned char* tempBitmap = new unsigned char[width*height];
        for (int i = 0; i < size; ++i)
            tempBitmap[i] = (unsigned char)(((double*)bitmap)[i]);
        img = QImage((const uchar*)tempBitmap, width, height, width*1, QImage::Format_Grayscale8);
    }
    //------------------------------------------------------------------------------------------//
    lastReceivedImage = img.convertToFormat(QImage::Format_RGB888).copy();
    imageIndex++;
    delete []tempBitmap;
    QString currentTime = QTime::currentTime().toString();
    ui->edtLastUpdateTime->setText(currentTime);
    //------------------------------------------------------------------------------------------//
    updateViewerImage();

    if (imageHistory.size() < ui->spbHistoryLimit->value())
    {
        QPixmap pixmap = QPixmap::fromImage(lastReceivedImage);
        QString itemName = QString::number(imageIndex) + QString(" ") + currentTime;

        double aspect = lastReceivedImage.width() / (double)lastReceivedImage.height();
        pixmap = pixmap.scaled(100/*w*/, 100.0 / aspect /*h*/);
        ui->lstWidgetItems->addItem(new QListWidgetItem(QIcon(pixmap), itemName));
        imageHistory[itemName] = lastReceivedImage;
    }
}

void plotter_image::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}


void plotter_image::on_spbImageScale_valueChanged(double arg1)
{
    ui->sliderScale->setValue(arg1);
    updateViewerImage();
}

void plotter_image::on_spbImageScale_editingFinished()
{
    updateViewerImage();
}

void plotter_image::updateViewerImage()
{
    if (lastReceivedImage.isNull())
        return;

    QImage img = lastReceivedImage.scaled( lastReceivedImage.size() * ui->spbImageScale->value() );
    if (ui->cbxIncomeOrder->currentIndex() == 0)
        img = img.transformed(QTransform().scale(1,-1).translate(0, -img.height()));

    {
        int h = img.size().height();
        int w = img.size().width();
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                QRgb pixel = img.pixel(x, y);
                img.setPixel(x, y, QColor(ui->cbxRed->isChecked()   ? qRed(pixel)   : 0,
                                          ui->cbxGreen->isChecked() ? qGreen(pixel) : 0,
                                          ui->cbxBlue->isChecked()  ? qBlue(pixel)  : 0
                                          ).rgb()
                            );
            }
        }
    }

    ui->lblViewImage->setPixmap(QPixmap::fromImage(img));
    ui->edtImageSizeInViewer->setText(QString("%1 x %2").arg(img.width()).arg(img.height()));
    ui->edtIncomeImageSize->setText(QString("w = %1, h = %2").arg(lastReceivedImage.width()).arg(lastReceivedImage.height()));
}
void plotter_image::on_cbxIncomeOrder_activated(int
                                                  //index
                                                  )
{
    updateViewerImage();
}

void plotter_image::on_cbxRed_clicked()
{
    updateViewerImage();
}

void plotter_image::on_cbxGreen_clicked()
{
    updateViewerImage();
}

void plotter_image::on_cbxBlue_clicked()
{
    updateViewerImage();
}

void plotter_image::on_sliderScale_valueChanged(int value)
{
    ui->spbImageScale->setValue(value);
}

void plotter_image::on_lstWidgetItems_itemDoubleClicked(QListWidgetItem *item)
{
    QString text = item->text();
    assert(imageHistory.find(item->text()) != imageHistory.end());
    lastReceivedImage = imageHistory[item->text()];
    updateViewerImage();
}

void plotter_image::on_btnLoadImage_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Save image dialog"), "", tr("PNG (*.png);;JPEG (*.jpg *.jpeg)" ));
    if (imagePath.isEmpty())
        return;

    QImage img(imagePath);
    img = img.convertToFormat(QImage::Format_RGB888);

    int h = img.size().height();
    int w = img.size().width();
    unsigned char* tempBuffer = new unsigned char[w*h*3];

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            QRgb pixel = img.pixel(x, y);
            tempBuffer[3*(y*w + x) + 0] = qRed(pixel);
            tempBuffer[3*(y*w + x) + 1] = qGreen(pixel);
            tempBuffer[3*(y*w + x) + 2] = qBlue(pixel);
        }
    }
    slotHandleBitmapInSpecifiedFormat(tempBuffer, w, h, w*h*3);
    delete []tempBuffer;
}

void plotter_image::on_btnCleanAllHistory_clicked()
{
    imageHistory.clear();
    ui->lstWidgetItems->clear();
}
