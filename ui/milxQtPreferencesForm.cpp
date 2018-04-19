/*=========================================================================
  The Software is copyright (c) Commonwealth Scientific and Industrial Research Organisation (CSIRO)
  ABN 41 687 119 230.
  All rights reserved.

  Licensed under the CSIRO BSD 3-Clause License
  You may not use this file except in compliance with the License.
  You may obtain a copy of the License in the file LICENSE.md or at

  https://stash.csiro.au/projects/SMILI/repos/smili/browse/license.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
=========================================================================*/
#include "milxQtPreferencesForm.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDesktopWidget>
#include <QScrollArea>

milxQtPreferencesForm::milxQtPreferencesForm(milxQtMain *theParent) : QDialog(theParent)
{
    ui.setupUi(this);

    setWindowModality(Qt::ApplicationModal); //block user input
    setWindowTitle(tr("sMILX Preferences"));
	setFixedSize(this->size());

    MainWindow = theParent;

    setupPages();
    createConnections();
}

milxQtPreferencesForm::~milxQtPreferencesForm()
{
    //dtor
}

void milxQtPreferencesForm::setupPages()
{
    ///General Page
	QWidget *generalPage = new QWidget;
	QListWidgetItem *generalPageItem = new QListWidgetItem(ui.wdtOptions);
		generalPageItem->setText(tr("General"));
		generalPageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	//Application options
	//Window size (text)
	QLabel *windowSizeLabel = new QLabel(tr("Preferred Window Size:"));
	QSize desktopSize = qApp->desktop()->availableGeometry().size();
		windowSizeEdit = new QSpinBox;
		windowSizeEdit->setMinimum(minWindowSize);
		windowSizeEdit->setMaximum(milx::Maximum<int>(desktopSize.width(), desktopSize.height()));
		windowSizeEdit->setValue(MainWindow->hasPreferredSubWindowSize());
	QHBoxLayout *windowSizeLayout = new QHBoxLayout;
		windowSizeLayout->addWidget(windowSizeLabel);
		windowSizeLayout->addWidget(windowSizeEdit);
	//Number of processors (text)
	QLabel *processorsLabel = new QLabel(tr("Preferred Max. Processors:"));
	processorsEdit = new QSpinBox;
		processorsEdit->setMinimum(1);
		processorsEdit->setMaximum(milx::NumberOfProcessors());
		processorsEdit->setValue(MainWindow->hasMaximumProcessors());
	QHBoxLayout *processorsLayout = new QHBoxLayout;
		processorsLayout->addWidget(processorsLabel);
		processorsLayout->addWidget(processorsEdit);
	//Magnification of Screenshots
	QLabel *magnifyLabel = new QLabel(tr("Screenshot Magnification Factor"));
	magnifyEdit = new QSpinBox;
		magnifyEdit->setMinimum(1);
		magnifyEdit->setMaximum(1024);
		magnifyEdit->setValue(MainWindow->hasScreenshotMagnifyFactor());
	QHBoxLayout *magnifyLayout = new QHBoxLayout;
		magnifyLayout->addWidget(magnifyLabel);
		magnifyLayout->addWidget(magnifyEdit);
	//Timestamp in log? (check)
	timestampCheckBox = new QCheckBox(tr("Show timestamp in logs"));
	timestampCheckBox->setChecked(MainWindow->isTimestampsPreferred());
	//Application theme options
	QLabel *themeLabel = new QLabel(tr("Theme:"));
		themeList = new QComboBox;
		themeList->addItem("Light");
		themeList->addItem("Dark");
		//Load additional themes
		QString currTheme = MainWindow->currentTheme();
		currTheme.replace(0, 1, currTheme[0].toUpper());
		themeList->setCurrentText(currTheme);
		editThemeButton = new QPushButton("Edit Theme");
	QHBoxLayout *themeLayout = new QHBoxLayout;
		themeLayout->addWidget(themeLabel);
		themeLayout->addWidget(themeList);
		themeLayout->addWidget(editThemeButton);
	//Application options layout
	QVBoxLayout *generalLayout = new QVBoxLayout;
		generalLayout->addLayout(windowSizeLayout);
		generalLayout->addLayout(processorsLayout);
		generalLayout->addLayout(magnifyLayout);
		generalLayout->addWidget(timestampCheckBox);
		generalLayout->addLayout(themeLayout);
		generalLayout->setAlignment(Qt::AlignTop);

	//General page layout
	generalPage->setLayout(generalLayout);
	
	//Add General options page to preferences
    ui.wdtPages->insertWidget(0, generalPage);
    ui.wdtPages->setCurrentWidget(generalPage);
    ui.wdtOptions->setCurrentItem(generalPageItem);
	
	///Workspace Page
	QWidget *workspacePage = new QWidget;
	QListWidgetItem *workspacePageItem = new QListWidgetItem(ui.wdtOptions);
		workspacePageItem->setText(tr("Workspace"));
		workspacePageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	//View options
	//Background colour (check)
	backgroundCheckBox = new QCheckBox(tr("Prefer white background"));
	backgroundCheckBox->setChecked(MainWindow->isWhiteBackgroundPreferred());
	//Human glyph (check)
	humanCheckBox = new QCheckBox(tr("Show human orientation glyph"));
	humanCheckBox->setChecked(MainWindow->isHumanGlyphPreferred());
	//View options layout
	QVBoxLayout *viewLayout = new QVBoxLayout;
		viewLayout->addWidget(backgroundCheckBox);
		viewLayout->addWidget(humanCheckBox);
	QGroupBox *viewGroup = new QGroupBox(tr("View Options"));
		viewGroup->setLayout(viewLayout);

	//Imaging options
	//Interpolation (check)
	interpolationCheckBox = new QCheckBox(tr("Interpolate images"));
	interpolationCheckBox->setChecked(MainWindow->isImageInterpolationPreferred());
	//Apply Orientation (check)
	orientationCheckBox = new QCheckBox(tr("Apply orientation to images"));
	orientationCheckBox->setChecked(MainWindow->isOrientationPreferred());
	//TODO Add custom colour maps

	//Imaging options layout
	QVBoxLayout *imagingLayout = new QVBoxLayout;
	imagingLayout->addWidget(interpolationCheckBox);
	imagingLayout->addWidget(orientationCheckBox);
	QGroupBox *imagingGroup = new QGroupBox(tr("Imaging"));
	imagingGroup->setLayout(imagingLayout);

	//Model options
	//Interpolation (check)
	interpolationModelCheckBox = new QCheckBox(tr("Interpolate (Phong Shading) Models"));
	interpolationModelCheckBox->setChecked(MainWindow->isModelInterpolationPreferred());
	///Scalar bar (Check)
	scalarBarCheckBox = new QCheckBox(tr("Always show scalar bar"));
	scalarBarCheckBox->setChecked(MainWindow->isScalarBarPreferred());
	//layout
	QVBoxLayout *ModelLayout = new QVBoxLayout;
	ModelLayout->addWidget(interpolationModelCheckBox);
	ModelLayout->addWidget(scalarBarCheckBox);
	QGroupBox *modelGroup = new QGroupBox(tr("Models"));
	modelGroup->setLayout(ModelLayout);

	//Colour Maps
	colourMapCheckBox = new QCheckBox(tr("Enable Custom Colour Maps"));
	colourMapCheckBox->setChecked(MainWindow->isColourMapEnabled());
	//layout
	QVBoxLayout *colourMapLayout = new QVBoxLayout;
	colourMapLayout->addWidget(colourMapCheckBox);
	QGroupBox *colourMapGroup = new QGroupBox(tr("Colour Maps"));
	colourMapGroup->setLayout(colourMapLayout);

	//Workspace page layout
	QVBoxLayout *workspacePageLayout = new QVBoxLayout;
	workspacePageLayout->addWidget(viewGroup);
	workspacePageLayout->addWidget(imagingGroup);
	workspacePageLayout->addWidget(modelGroup);
	workspacePageLayout->addWidget(colourMapGroup);
	workspacePageLayout->setAlignment(Qt::AlignTop);
	workspacePage->setLayout(workspacePageLayout);

	//Add General options page to preferences
	ui.wdtPages->insertWidget(1, workspacePage);

    ///Plugins Page
    QWidget *pluginsPage = new QWidget;
	QVBoxLayout *pluginsPageLayout = new QVBoxLayout;
    QListWidgetItem *pluginsPageItem = new QListWidgetItem(ui.wdtOptions);
        pluginsPageItem->setText(tr("Plugins"));
        pluginsPageItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	// Check if no plugins
	if (!MainWindow->getPlugins().size()) {
		noPluginMsg = new QLabel(tr("There are currently no plugins installed."));
		pluginsPageLayout->addWidget(noPluginMsg);
		pluginsPageLayout->setAlignment(Qt::AlignCenter);
	} else {
		///List and Disable
		QListWidget *pluginsList = new QListWidget;
		pluginsList->setDisabled(true);

		foreach(QPointer<milxQtPluginInterface> plugin, MainWindow->getPlugins())
		{
			QListWidgetItem *pluginsItem = new QListWidgetItem(pluginsList);
			pluginsItem->setText(plugin->name());
			pluginsItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}
		pluginsPageLayout->addWidget(pluginsList);
	}
    //Layout	
    pluginsPage->setLayout(pluginsPageLayout);
    ui.wdtPages->insertWidget(2, pluginsPage);
}

void milxQtPreferencesForm::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
//    cout << "Index of Page: " << ui.wdtOptions->row(current) << endl;
    ui.wdtPages->setCurrentIndex(ui.wdtOptions->row(current));
}

void milxQtPreferencesForm::changeTheme(int)
{
	//QString msg = QString("Changing Theme to: " + themeList->currentText().toLower());
	//MainWindow->printDebug(msg);
	
	QString themeFile = QString(":/resources/styles/" + themeList->currentText().toLower() + ".qss");
	QFile qss(themeFile);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
	MainWindow->update();
}


void milxQtPreferencesForm::accept()
{
    //General
    MainWindow->preferWhiteBackground(backgroundCheckBox->isChecked());
    MainWindow->preferHumanGlyph(humanCheckBox->isChecked());
    MainWindow->preferSubWindowSize(windowSizeEdit->value());
    MainWindow->preferMaximumProcessors(processorsEdit->value());
    MainWindow->preferScreenshotMagnifyFactor(magnifyEdit->value());
    MainWindow->preferTimestamps(timestampCheckBox->isChecked());
	MainWindow->setCurrentTheme(themeList->currentText().toLower());
    
	//Imaging
    MainWindow->preferImageInterpolation(interpolationCheckBox->isChecked());
    MainWindow->preferOrientation(orientationCheckBox->isChecked());
    
	//Models
    MainWindow->preferModelInterpolation(interpolationModelCheckBox->isChecked());
    MainWindow->preferScalarBar(scalarBarCheckBox->isChecked());

	//Colour Maps
	MainWindow->activateColourMaps(colourMapCheckBox->isChecked());
	MainWindow->writeSettings();
	QDialog::accept();
}

void milxQtPreferencesForm::createConnections()
{
    connect(ui.wdtOptions, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
	connect(this->themeList, SIGNAL(currentIndexChanged(int)),
		this, SLOT(changeTheme(int)));
}