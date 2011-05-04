/*
 * medViewerToolBoxViewProperties.cpp
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#include <medViewerToolBoxViewProperties.h>

#include <dtkCore/dtkAbstractData.h>
#include <medCore/medAbstractView.h>
#include <medGui/medToolBoxTab.h>

class medViewerToolBoxViewPropertiesPrivate
{
    public:
        medAbstractView *view;
        QTreeWidget * propertiesTree;
        QStringList lutList;
        int currentLayer;
        QWidget * twoLayersWidget;
        QSlider * slider;
        QString textLayer0;
        QString textLayer1;
        QPushButton * switchLayersButton;
};

medViewerToolBoxViewProperties::medViewerToolBoxViewProperties(QWidget *parent) :
    medToolBox(parent), d(new medViewerToolBoxViewPropertiesPrivate)
{
    d->view = 0;
    d->currentLayer = 0;
    d->textLayer0 = tr("Switch to layer 0 only");
    d->textLayer1 = tr("Switch to layer 1 only");
    

    d->lutList << "Default" << "Black & White" << "Black & White Inversed" << "Spectrum" << "Hot Metal" << "Hot Green"
               << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2" << "Asymmetry" << "P-Value" << "Red Black Alpha"
               << "Green Black Alpha" << "Blue Black Alpha" << "Muscles & Bones" << "Bones" << "Red Vessels"
               << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";

    d->propertiesTree = new QTreeWidget(this);

    d->propertiesTree->setColumnCount(3);
    d->propertiesTree->setColumnWidth(0,50);
    d->propertiesTree->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList headers;
    headers << "Layer" << "Name" << "Value";
    d->propertiesTree->setHeaderLabels(headers);
    d->propertiesTree->setAnimated(true);
    d->propertiesTree->setAlternatingRowColors(true);
    d->propertiesTree->setRootIsDecorated(false);
    d->propertiesTree->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setTitle("View Properties");
    this->addWidget(d->propertiesTree);

    QObject::connect(d->propertiesTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
    QObject::connect(d->propertiesTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextTreeMenu(QPoint)));
    
    //add 2 layers opacity slider and switcher.
    d->twoLayersWidget = new QWidget(this);
    QGridLayout * twoLayersLayout = new QGridLayout();
    d->twoLayersWidget->setLayout(twoLayersLayout);
    
    d->slider = new QSlider(Qt::Horizontal,this);
    d->slider->setRange(0,100);
    d->slider->setValue(50);
    QObject::connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(on2LayersOpacitySliderSet(int)));
    twoLayersLayout->addWidget(new QLabel(tr("Layers' opacity:"),this),
                               0,0,1,3,Qt::AlignLeft);
    twoLayersLayout->addWidget(new QLabel(tr("Layer 0"),this),1,0,1,1);
    twoLayersLayout->addWidget(d->slider,1,1,1,1);
    twoLayersLayout->addWidget(new QLabel(tr("Layer 1"),this),1,2,1,1);
    d->switchLayersButton = new QPushButton(d->textLayer0,this);
    twoLayersLayout->addWidget(d->switchLayersButton,2,0,1,3);
    
    QObject::connect(d->switchLayersButton,SIGNAL(clicked()),
                     this,SLOT(onSwitchLayersButtonClicked()));
    
    this->addWidget(d->twoLayersWidget);
    d->twoLayersWidget->hide();
    
    this->hide();
    
}

medViewerToolBoxViewProperties::~medViewerToolBoxViewProperties(void)
{
    delete d;
    d = NULL;
}

void
medViewerToolBoxViewProperties::update(dtkAbstractView *view)
{
    if ((d->view) && (d->view != dynamic_cast<medAbstractView *> (view)) )
    {
        QObject::disconnect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)));
        QObject::disconnect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()));

        this->onViewClosed();
    }

    if (medAbstractView *medView = dynamic_cast<medAbstractView *> (view)) {
        if ((d->view == dynamic_cast<medAbstractView *> (view)))
            return;

        d->view = medView;

        for (int i = 0; i < d->view->layerCount(); i++)
        {
            QTreeWidgetItem * layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
            layerItem->setText(0, QString::number(i));
            layerItem->setIcon(0,QIcon(":icons/layer.png"));

            QTreeWidgetItem * visibleItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
            visibleItem->setText(1, "Visible");
            QCheckBox * visibleBox = new QCheckBox();
            visibleBox->setChecked(d->view->visibility(i));
            d->propertiesTree->setItemWidget(visibleItem, 2, visibleBox);
            QObject::connect(visibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));

            QTreeWidgetItem * opacityItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
            opacityItem->setText(1, "Opacity");
            QSlider * opacityBox = new QSlider(Qt::Horizontal);
            opacityBox->setRange(0,100);
            opacityBox->setValue(d->view->opacity(i) * 100);
            d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);
            QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));

            QTreeWidgetItem * lutItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
            lutItem->setText(1, "LUT");
            QComboBox * lutBox = new QComboBox();
            lutBox->setFocusPolicy(Qt::NoFocus);
            lutBox->addItem("Default");
            lutBox->addItem("Black & White");
            lutBox->addItem("Black & White Inversed");
            lutBox->addItem("Spectrum");
            lutBox->addItem("Hot Metal");
            lutBox->addItem("Hot Green");
            lutBox->addItem("Hot Iron");
            lutBox->addItem("GE");
            lutBox->addItem("Flow");
            lutBox->addItem("Loni");
            lutBox->addItem("Loni 2");
            lutBox->addItem("Asymmetry");
            lutBox->addItem("P-Value");
            lutBox->addItem("Red Black Alpha");
            lutBox->addItem("Green Black Alpha");
            lutBox->addItem("Blue Black Alpha");
            lutBox->addItem("Muscles & Bones");
            lutBox->addItem("Bones");
            lutBox->addItem("Red Vessels");
            lutBox->addItem("Cardiac");
            lutBox->addItem("Gray Rainbow");
            lutBox->addItem("Stern");
            lutBox->addItem("Black Body");

            d->propertiesTree->setItemWidget(lutItem, 2, lutBox);
            lutBox->setCurrentIndex(0);

	    // here we should not change the view settings but rather populate
	    // the GUI with the current view settings
	    /*
	      d->view->setCurrentLayer(i);
	      d->view->setProperty("LookupTable", "Default");
	      d->view->update();
	    */
	    
            QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));

            d->propertiesTree->collapseAll();
        }

        QObject::connect(d->view, SIGNAL(dataAdded(int)), this, SLOT(onDataAdded(int)), Qt::UniqueConnection);
        QObject::connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
        
        raiseSlider(d->view->layerCount() == 2);
       
    }
}

void
medViewerToolBoxViewProperties::onDataAdded(int layer)
{
    if (!d->view)
        return;

//        dtkAbstractData * data = d->view->dataInList(layer);
//        const QString seriesName = data->metaDataValues(tr("SeriesDescription"))[0];
    //    const QString seriesName = data->name();

    if (d->view->layerCount() == 1)
        return;

    QTreeWidgetItem * layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    layerItem->setText(0, QString::number(layer));
    layerItem->setIcon(0,QIcon(":icons/layer.png"));

    QTreeWidgetItem * visibleItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
    visibleItem->setText(1, "Visible");
    QCheckBox * visibleBox = new QCheckBox();
    visibleBox->setChecked(true);
    d->propertiesTree->setItemWidget(visibleItem, 2, visibleBox);
    QObject::connect(visibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));

    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
    opacityItem->setText(1, "Opacity");
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->view->opacity(layer) * 100);
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));

    QTreeWidgetItem * lutItem = new QTreeWidgetItem(layerItem, QTreeWidgetItem::UserType+2);
    lutItem->setText(1, "LUT");
    QComboBox * lutBox = new QComboBox();
    lutBox->setFocusPolicy(Qt::NoFocus);
    lutBox->addItem("Default");
    lutBox->addItem("Black & White");
    lutBox->addItem("Black & White Inversed");
    lutBox->addItem("Spectrum");
    lutBox->addItem("Hot Metal");
    lutBox->addItem("Hot Green");
    lutBox->addItem("Hot Iron");
    lutBox->addItem("GE");
    lutBox->addItem("Flow");
    lutBox->addItem("Loni");
    lutBox->addItem("Loni 2");
    lutBox->addItem("Asymmetry");
    lutBox->addItem("P-Value");
    lutBox->addItem("Red Black Alpha");
    lutBox->addItem("Green Black Alpha");
    lutBox->addItem("Blue Black Alpha");
    lutBox->addItem("Muscles & Bones");
    lutBox->addItem("Bones");
    lutBox->addItem("Red Vessels");
    lutBox->addItem("Cardiac");
    lutBox->addItem("Gray Rainbow");
    lutBox->addItem("Stern");
    lutBox->addItem("Black Body");

    d->propertiesTree->setItemWidget(lutItem, 2, lutBox);
    lutBox->setCurrentIndex(0);

    d->view->setCurrentLayer(layer);
    // we should not change the view settigns but adapt the GUI to them
    // d->view->setProperty("LookupTable", "Default");
    // d->view->update();

    QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));

    d->propertiesTree->collapseAll();
    raiseSlider(d->view->layerCount() == 2,0.5);
}

void
medViewerToolBoxViewProperties::onViewClosed(void)
{
    d->propertiesTree->clear();
    d->view = 0;
}

void
medViewerToolBoxViewProperties::onVisibilitySet(int state)
{
    if (!d->view)
        return;

    if (state == Qt::Checked)
        d->view->setVisibility(1, d->currentLayer);
    else
        d->view->setVisibility(0, d->currentLayer);
    d->view->update();
}

void
medViewerToolBoxViewProperties::onOpacitySet(double opacity)
{
    if (!d->view)
        return;

    d->view->setOpacity(opacity, d->currentLayer);
    d->view->update();
}

void
medViewerToolBoxViewProperties::onOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;
    d->view->setOpacity(d_opacity, d->currentLayer);
    d->view->update();
}

void
medViewerToolBoxViewProperties::on2LayersOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;
    d->view->setOpacity(d_opacity, 1);
    d->view->setOpacity(1.0 - d_opacity, 0);
    d->view->update();
}


void
medViewerToolBoxViewProperties::onLUTChanged(int index)
{
    if (!d->view)
        return;

    d->view->setCurrentLayer(d->currentLayer);
    d->view->setProperty("LookupTable", d->lutList.at(index));
    d->view->update();
}

void
medViewerToolBoxViewProperties::onItemClicked(QTreeWidgetItem * item, int column)
{
    d->propertiesTree->clearSelection();
    if (item->type() == QTreeWidgetItem::UserType + 1)
    {
        if (item->isExpanded())
            d->propertiesTree->collapseItem(item);
        else
        {
            d->propertiesTree->collapseAll();
            d->propertiesTree->expandItem(item);
            d->currentLayer = item->text(0).toInt();
        }
    }
}

void medViewerToolBoxViewProperties::onContextTreeMenu( const QPoint point )
{
    QTreeWidgetItem * item = 0;
    item = d->propertiesTree->itemAt(point);

    if (!item)
        return;

    if (item->type() != QTreeWidgetItem::UserType + 1)
        return;

    if (item->text(0).toInt() == 0)
        return;

    d->currentLayer = item->text(0).toInt();

    item->setSelected(true);

    QMenu * menu = new QMenu(d->propertiesTree);
    QAction * deleteLayer = new QAction(this);
    deleteLayer->setIconVisibleInMenu(true);
    deleteLayer->setText(tr("Delete"));
    deleteLayer->setIcon(QIcon(":icons/cross.png"));
    QObject::connect(deleteLayer, SIGNAL(triggered()), this, SLOT(onDeleteLayer()));
    menu->addAction(deleteLayer);

    menu->exec(d->propertiesTree->mapToGlobal(point));
    delete menu;
}

void medViewerToolBoxViewProperties::onDeleteLayer()
{
    d->view->removeOverlay(d->currentLayer);
    d->view->update();

    if (d->currentLayer != 0)
        d->propertiesTree->invisibleRootItem()->removeChild(d->propertiesTree->selectedItems()[0]);

    d->propertiesTree->clearSelection();
    raiseSlider(d->view->layerCount() == 2);
}

void medViewerToolBoxViewProperties::clear()
{
    d->currentLayer = 0;
    onViewClosed();
}

void medViewerToolBoxViewProperties::raiseSlider(bool isVisible,double opacity)
{
    if (isVisible)
    {
        if (opacity < 0 || opacity > 1)
        {
            opacity = d->view->opacity(1);
        }
            
        //set opacity to layer 1's
        d->slider->setValue( opacity * 100);
        d->twoLayersWidget->show();
    }
    else
    {
        d->twoLayersWidget->hide();
    }
}

void medViewerToolBoxViewProperties::onSwitchLayersButtonClicked()
{
    if (d->switchLayersButton->text() == d->textLayer0)
    {
        d->switchLayersButton->setText(d->textLayer1);
        raiseSlider(true,0.0);
    }
    else
    {
        d->switchLayersButton->setText(d->textLayer0);
        raiseSlider(true,1.0);
    }
}