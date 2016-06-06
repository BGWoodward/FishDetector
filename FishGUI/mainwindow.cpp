#include <mainwindow.h>

MainWindow::MainWindow(QWidget *parent)
    :QWidget(parent)
	{
        ui.reset(new Ui::MainWidget);
        fIndex = 0;
        nextID = 1;
        player.reset(new Player());
        myFishList.clear();
        QObject::connect(player.get(), SIGNAL(processedImage(QImage)),
                         this, SLOT(updatePlayerUI(QImage)));
		ui->setupUi(this);
        disableControls();
		QStringList typeList;
		typeList.append("Round");
		typeList.append("Flat");
		typeList.append("Skate");
		typeList.append("Other");
		ui->typeMenu->addItems(typeList);
        QObject::connect(ui->typeMenu, SIGNAL(currentIndexChanged(int)),
                         this, SLOT(updateSubTypeMenu(int)));
		ui->typeMenu->setCurrentIndex(3);
        QObject::connect(ui->goToFishVal,SIGNAL(returnPressed()),
                         this, SLOT(goToFish()));

        auto next_button = ui->navigator->findChild<QPushButton *>("next_button");
        connect(next_button, SIGNAL(clicked()), this, SLOT(on_plusOneFrame_clicked()));

        auto prev_button = ui->navigator->findChild<QPushButton *>("prev_button");
        connect(prev_button, SIGNAL(clicked()), this, SLOT(on_minusOneFrame_clicked()));

        auto add_region_button = ui->navigator->findChild<QPushButton *>("add_region_button");
        connect(add_region_button, SIGNAL(clicked()), this, SLOT(on_addRegion_clicked()));

        auto remove_region_button = ui->navigator->findChild<QPushButton *>("remove_region_button");
        connect(remove_region_button, SIGNAL(clicked()), this, SLOT(on_removeRegion_clicked()));

        auto next_and_copy_button = ui->navigator->findChild<QPushButton *>("next_with_copy_button");
        connect(next_and_copy_button, SIGNAL(clicked()), this, SLOT(on_nextAndCopy_clicked()));
	}

void MainWindow::nextFrame()
{
    updateImage(player->nextFrame()); // get next frame
    processAnnotations(player->getCurrentFrame());
}

void MainWindow::prevFrame()
{
    updateImage(player->prevFrame()); // get next frame
    processAnnotations(player->getCurrentFrame());
}

void MainWindow::processAnnotations(uint64_t frame) {
    // remove old annotations
    for (auto ann : currentAnnotations) {
        scene->removeItem(ann);
    }
    currentAnnotations.clear();
    // add new annotations
    for (auto ann : document->getAnnotations(frame)) {
        auto rect = QRectF(ann.second->area.x, ann.second->area.y, ann.second->area.w, ann.second->area.h);
        auto region = new AnnotatedRegion(ann.first, ann.second, rect);
        scene->addItem(region);
        currentAnnotations.push_back(region);
    }
}

void MainWindow::gotoFrame() {

}

void MainWindow::updatePlayerUI(QImage img)
{
	if (!img.isNull())
	{
		displayImage->setPixmap(QPixmap::fromImage(img));
		scene->setSceneRect(img.rect());
		ui->videoWindow->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
		ui->videoSlider->setValue(player->getCurrentFrame());
		ui->currentTime->setText(getFormattedTime((int)player->
			getCurrentFrame() / (int)player->getFrameRate()));
        processAnnotations(player->getCurrentFrame());
	}
}

void MainWindow::updateSubTypeMenu(int typeIndex)
{
	QStringList sTypeList;
    ui->subTypeMenu->blockSignals(true);
	ui->subTypeMenu->clear();
	if (typeIndex == 0) {
		sTypeList.append("Round");
		sTypeList.append("Haddock");
		sTypeList.append("Cod");
		sTypeList.append("Whiting");
		sTypeList.append("Red Hake");
		sTypeList.append("Pollock");
		sTypeList.append("Herring");
		sTypeList.append("Unknown");
	}
	else if (typeIndex == 1) {
		sTypeList.append("Flat");
		sTypeList.append("Yellowtail");
		sTypeList.append("Windowpane");
		sTypeList.append("Summer");
		sTypeList.append("Winter");
		sTypeList.append("Fourspot");
		sTypeList.append("Grey Sole");
		sTypeList.append("Halibut");
		sTypeList.append("Unknown");
	}
	else if (typeIndex == 2) {
		sTypeList.append("Skate");
		sTypeList.append("Barndoor");
		sTypeList.append("Unknown");
	}
	else if (typeIndex == 3) {
		sTypeList.append("Other");
		sTypeList.append("Dogfish");
		sTypeList.append("Monkfish");
		sTypeList.append("Lobster");
		sTypeList.append("Scallop");
	}
	ui->subTypeMenu->addItems(sTypeList);
    ui->subTypeMenu->setCurrentIndex(0);
    ui->subTypeMenu->blockSignals(false);
}

void MainWindow::goToFish()
{
    int fNumber = ui->goToFishVal->text().toInt();
    if ((fNumber > 0) && (fNumber < int(myFishList.size())))
    {
        listPos = myFishList.begin()+fNumber-1;
        ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
        ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
        updateVecIndex();
    }
    else
    {
        listPos = myFishList.end()-1;
        ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
        ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
        updateVecIndex();
    }
}

QString MainWindow::getFormattedTime(int timeInSeconds)
{
	int seconds = (int) (timeInSeconds) % 60;
	int minutes = (int)((timeInSeconds / 60) % 60);
	int hours = (int)((timeInSeconds / (60 * 60)) % 24);
	QTime t(hours, minutes, seconds);
	if (hours == 0)
		return t.toString("mm:ss");
	else
		return t.toString("h:mm:ss");
}

void MainWindow::on_LoadVideo_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open Video"), ".",
		tr("Video Files (*.avi *.mpg *.mp4)"));
    QFileInfo name = filename;
	if (!filename.isEmpty())
	{
        if (!player->loadVideo(filename.toLatin1().data()))
		{
			QMessageBox msgBox;
			msgBox.setText("The selected video could not be opened!");
			msgBox.exec();
		}
		else
        {
            enableControls();
            this->setWindowTitle(name.fileName());
			ui->Play->setEnabled(true);
			ui->videoSlider->setEnabled(true);
			ui->videoSlider->setMaximum(player->getNumberOfFrames());
			ui->totalTime->setText(getFormattedTime((int)player->
				getNumberOfFrames() / (int)player->getFrameRate()));
            QImage firstImage = player->getOneFrame();
            scene.reset(new QGraphicsScene(this));
			displayImage = scene->addPixmap(QPixmap::fromImage(firstImage));
			scene->setSceneRect(firstImage.rect());
            ui->videoWindow->setScene(scene.get());
            ui->videoWindow->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
            ui->videoWindow->show();
            ui->currentSpeed->setText("Current Speed: 100%");
            ui->Play->setFocus();
            // TODO: should first try to load this if the data
            // file exists
            document.reset(new FishDetector::Document());
		}
	}
}

void MainWindow::on_loadAnnotate_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Annotation File"), ".",
        tr("CSV Files (*.csv)"));
    std::string filenameBase = base_name(filename.toStdString());
    std::string filenameBaseNoExt = remove_extension(filenameBase);
    std::string filenameJSON = remove_extension(filename.toStdString()) + ".json";
    ifstream inputJSON(filenameJSON.c_str(), ios::in);
    if (!inputJSON.fail()) {
        FishDetector::Document* newDoc = new FishDetector::Document(FishDetector::deserialize<FishDetector::Document>(inputJSON));
        document.reset(newDoc);
    }
    std::string filenameBaseNoReviewer = remove_reviewer(filenameBaseNoExt);
    QString qFilename = QString::fromStdString(filenameBaseNoReviewer);
    ui->fileNameValue->setText(qFilename);
    ifstream inFile(filename.toLatin1().data());
    string line, tripID, reviewer, towType, fishNum, fishType, species;
    int frame, towNum, curID;
    double timeInVid;
    getline(inFile,line);
    line.clear();
    bool first = true;
    while(getline(inFile,line))
    {
        stringstream linestream(line);
        string tempToken;
        std::getline(linestream,tripID,',');
        std::getline(linestream,tempToken,',');
        string strtowNum = tempToken;
        stringstream tempConvert(tempToken);
        tempConvert >> towNum;
        tempToken.clear();
        tempConvert.str("");
        tempConvert.clear();
        std::getline(linestream,reviewer,',');
        std::getline(linestream,towType,',');
        std::getline(linestream,fishNum,',');
        std::getline(linestream,fishType,',');
        std::getline(linestream,species,',');
        std::getline(linestream,tempToken,',');
        if (first)
        {
            QString qreviewer = QString::fromStdString(reviewer);
            ui->reviewerNameValue->setText(qreviewer);
            QString qtripID = QString::fromStdString(tripID);
            ui->tripIDValue->setText(qtripID);
            QString qtowID = QString::fromStdString(strtowNum);
            ui->towIDValue->setText(qtowID);
        }
        first = false;
        tempConvert << tempToken;
        tempConvert >> frame;
        tempToken.clear();
        tempConvert.str("");
        tempConvert.clear();
        std::getline(linestream,tempToken,',');
        tempConvert << tempToken;
        tempConvert >> timeInVid;
        tempToken.clear();
        tempConvert.str("");
        tempConvert.clear();
        FishTypeEnum fType = getFishType(fishType);
        curID = std::stoi(fishNum,nullptr,10);
        unique_ptr<Fish> tempFish(new Fish(fType,frame,curID));
        if (curID >= nextID) nextID = curID + 1;
        //std::cout<<nextID<<std::endl;
        tempFish->setFishSubType(getFishSpecies(fType,species));
        myFishList.push_back(*tempFish);
        linestream.str("");
        linestream.clear();
    }
    inFile.close();
    ui->totalFishVal->setText(QString::number(myFishList.size()));
    listPos = myFishList.begin();
    ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
    ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
    updateVecIndex();
}

void MainWindow::on_saveAnnotate_clicked()
{

    QString dirName = QFileDialog::getExistingDirectory(this,tr("Choose save directory"));
    string filename;
    string filenameJSON;
    filename = dirName.toStdString() + "/" + filename + ui->fileNameValue->text().toStdString() + "_" + ui->reviewerNameValue->text().toStdString();
    filenameJSON = filename + ".json";
    filename = filename + ".csv";
    std::ofstream jsonFile (filenameJSON.c_str(), std::ofstream::out);
    FishDetector::serialize(*document, jsonFile);
    ofstream outFile(filename);
    outFile << "Trip_ID" << "," << "Tow_Number" << "," << "Reviewer" << "," << "Tow_Type" << ",";
    outFile << "Fish_Number" << "," << "Fish_Type" << "," << "Species" << "," << "Frame" << "," << "Time_In_Video" << std::endl;
    string towStatus;
    if (ui->towStatus->isChecked())
    {
        towStatus = "Open";
    }
    else
    {
        towStatus = "Closed";
    }
    int fishCount = 1;
    for(auto it = myFishList.begin(); it != myFishList.end(); ++it) {
        outFile << ui->tripIDValue->text().toStdString() << "," << ui->towIDValue->text().toStdString() << "," << ui->reviewerNameValue->text().toStdString() << "," << towStatus << ",";
        outFile << it->getID() << "," << getFishTypeString(it->getFishType()) << ",";
        outFile << getFishSpeciesString(it->getFishType(),it->getFishSubType()) << ",";
        outFile << it->frameCounted << ",";
        outFile << (double) it->frameCounted / player->getFrameRate() / 60.0 / 60.0 << std::endl;
        fishCount++;
    }
    outFile.close();
}

void MainWindow::on_Play_clicked()
{
	if (player->isStopped())
	{
		player->Play();
		ui->Play->setText(tr("Stop"));
	}
	else
	{
		player->Stop();
		ui->Play->setText(tr("Play"));
	}
}

void MainWindow::on_videoSlider_sliderPressed()
{
	player->Stop();
}

void MainWindow::on_videoSlider_sliderReleased()
{
	player->Play();
    ui->Play->setText(tr("Stop"));
}

void MainWindow::on_videoSlider_sliderMoved(int position)
{
	player->setFrame(position);
	ui->currentTime->setText(getFormattedTime(position / (int)player->getFrameRate()));
}

void MainWindow::on_SpeedUp_clicked()
{
	player->speedUp();
    QString tempSpeed;
    tempSpeed.setNum((int)(player->getCurrentSpeed()));
    ui->currentSpeed->setText("Current Speed: " + tempSpeed + "%");
}

void MainWindow::on_SlowDown_clicked()
{
	player->slowDown();
    QString tempSpeed;
    tempSpeed.setNum((int)(player->getCurrentSpeed()));
    ui->currentSpeed->setText("Current Speed: " + tempSpeed + "%");
}

void MainWindow::updateImage(const QImage &image)
{
    if (!player->isStopped())
	{
		player->Stop();
		ui->Play->setText(tr("Play"));
	}

    displayImage->setPixmap(QPixmap::fromImage(image));
    scene->setSceneRect(image.rect());
    ui->videoWindow->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    ui->videoSlider->setValue(player->getCurrentFrame());
    ui->currentTime->setText(getFormattedTime((int)player->
        getCurrentFrame() / (int)player->getFrameRate()));
}

void MainWindow::on_minusOneFrame_clicked()
{
    prevFrame();
}

void MainWindow::on_plusOneFrame_clicked()
{
    nextFrame();
}

void MainWindow::on_addRound_clicked()
{
	addFish((FishTypeEnum) ROUND);
}

void MainWindow::on_addFlat_clicked()
{
	addFish((FishTypeEnum) FLAT);
}

void MainWindow::on_addSkate_clicked()
{
	addFish((FishTypeEnum) SKATE);
}

void MainWindow::on_addOther_clicked()
{
	addFish((FishTypeEnum) OTHER);
}

void MainWindow::on_prevFish_clicked()
{
    if (listPos!=myFishList.begin())
    {
        listPos = listPos-1;
        ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
        ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
        updateVecIndex();
    }
}

void MainWindow::on_nextFish_clicked()
{
    if (listPos!=myFishList.end()-1)
    {
        listPos = listPos+1;
        ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
        ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
        updateVecIndex();
    }
}

void MainWindow::on_goToFrame_clicked()
{
    if (!player==NULL)
    {
        if (!player->isStopped())
        {
            player->Stop();
            ui->Play->setText(tr("Play"));
        }
        QImage image = player->setFrame(listPos->getFrameCounted());

        if (!image.isNull())
        {
            displayImage->setPixmap(QPixmap::fromImage(image));
            scene->setSceneRect(image.rect());
            ui->videoWindow->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
            ui->videoSlider->setValue(player->getCurrentFrame());
            ui->currentTime->setText(getFormattedTime((int)player->
                getCurrentFrame() / (int)player->getFrameRate()));
        }
        processAnnotations(player->getCurrentFrame());
    }
}

void MainWindow::on_removeFish_clicked()
{
    /*Steps to removing fish:
     *
     * 1. Remove any regions from scene
     * 2. Remove all frame annotations with ID
     * 3. Remove annotation with ID
     * 4. Remove from fish list
     */
    if (myFishList.begin() != myFishList.end()) {
        //auto id = uint64_t(listPos - myFishList.begin()+1);
        auto id = listPos->getID();

        auto it = find_if(currentAnnotations.begin(), currentAnnotations.end(), \
                          [&id](AnnotatedRegion* obj) {return obj->getUID() == id;});
        if (it != currentAnnotations.end()) {
            scene->removeItem(*it);
            currentAnnotations.erase(it);
        }
        document->removeFrameAnnotation(id);
        document->removeAnnotation(id);
        listPos = myFishList.erase(listPos);
        if (listPos == myFishList.end())
            listPos = myFishList.end()-1;
        updateVecIndex();
        ui->totalFishVal->setText(QString::number(myFishList.size()));
        ui->typeMenu->setCurrentIndex((int) listPos->getFishType());
        ui->subTypeMenu->setCurrentIndex((int) listPos->getFishSubType());
    }
}

void MainWindow::on_typeMenu_currentIndexChanged(int tIdx)
{
    if (!myFishList.empty())
        listPos->setFishType((FishTypeEnum) tIdx);
}

void MainWindow::on_subTypeMenu_currentIndexChanged(int sIdx)
{
    if (!myFishList.empty())
        listPos->setFishSubType(sIdx);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    QString keypress = e->text();
    int keycode;
    if (keypress == "f")
        keycode = 0;
    else if (keypress == "r")
        keycode = 1;
    else if (keypress == "s")
        keycode = 2;
    else if (keypress == "t")
        keycode = 3;
    else if (keypress == "n")
        keycode = 4;
    else if (keypress == "b")
        keycode = 5;
    else if (keypress == "m")
        keycode = 6;
    else if (keypress == "a")
        keycode = 7;
    else if (keypress == "q")
        keycode = 8;
    else if (keypress == "p")
        keycode = 9;
    else
        keycode = 10;

    switch (keycode)
    {
    case 0:
        ui->addFlat->animateClick();
        ui->Play->setFocus();
        break;
    case 1:
        ui->addRound->animateClick();
        ui->Play->setFocus();
        break;
    case 2:
        ui->addSkate->animateClick();
        ui->Play->setFocus();
        break;
    case 3:
        ui->addOther->animateClick();
        ui->Play->setFocus();
        break;
    case 4:
        ui->navigator->findChild<QPushButton *>("next_with_copy_button")->animateClick();
        ui->Play->setFocus();
        break;
    case 5:
        ui->navigator->findChild<QPushButton *>("prev_button")->animateClick();
        ui->Play->setFocus();
        break;
    case 6:
        ui->navigator->findChild<QPushButton *>("next_button")->animateClick();
        ui->Play->setFocus();
        break;
    case 7:
        ui->navigator->findChild<QPushButton *>("add_region_button")->animateClick();
        ui->Play->setFocus();
        break;
    case 8:
        ui->navigator->findChild<QPushButton *>("remove_region_button")->animateClick();
        ui->Play->setFocus();
        break;
    case 9:
        ui->Play->animateClick();
        break;
    case 10:
        QWidget::keyPressEvent(e);
        break;
    }
}

void MainWindow::on_addRegion_clicked()
{
    FishDetector::Rect area(0, 0, 100, 100);
    auto fishID = uint64_t(listPos->getID());
    auto frame = uint64_t(player->getCurrentFrame());
    //First check to see if there's an annotation for this ID already.
    if (document->keyExists(fishID))
    {
        removeRegion(fishID, frame);
    }
    else {
        document->addAnnotation(fishID);
    }
    auto loc = document->addAnnotationLocation(fishID, frame, area);
    auto annotationArea = new AnnotatedRegion(fishID,loc, QRect(0, 0, 100, 100));
    currentAnnotations.push_back(annotationArea);
    scene->addItem(annotationArea);
}

void MainWindow::on_removeRegion_clicked()
{
    auto fishID = uint64_t(listPos->getID());
    auto frame = uint64_t(player->getCurrentFrame());
    removeRegion(fishID, frame);
}

void MainWindow::removeRegion(std::uint64_t id, std::uint64_t frame) {
    //First check to see if there's an annotation for this ID already.
    if (document->keyExists(id))
    {
        //check to see if there's an annotation location for this frame already
        auto currentAnn = document->getAnnotation(id);
        if (currentAnn->frameHasAnn(frame)) {
            currentAnn->removeFrameAnn(frame);
            document->removeFrameAnnotation(id, frame);
            auto it = find_if(currentAnnotations.begin(), currentAnnotations.end(), \
                              [&id](AnnotatedRegion* obj) {return obj->getUID() == id;});
            if (it != currentAnnotations.end()) {
                scene->removeItem(*it);
                currentAnnotations.erase(it);
            }
        }
    }
    //This function will do nothing if there's no annotation to erase
}

void MainWindow::on_nextAndCopy_clicked()
{
    // remove old annotations
    for (auto ann : currentAnnotations) {
        scene->removeItem(ann);
    }
    currentAnnotations.clear();
    auto prevFrame = player->getCurrentFrame();
    updateImage(player->nextFrame());
    //Currently delete existing annotations and overwrites. Want to change
    //into next and copy current fish only. Desired behavior will be to
    //delete any current annotation associated with the fish and then
    //copy over the annotation from the current frame.

    //check this frame for annotations. We're going to delete anything
    //that currently exists.
    for (auto ann: document->getAnnotations()) {
        auto id = ann.first;
        //Here is where we'll check the id against the current fish. If
        //not equal, then just do nothing
        if(listPos->getID() == id) {
            removeRegion(id, prevFrame+1);
            break;
        }
    }
    // copy annotations
    for (auto ann : document->getAnnotations(prevFrame)) {
        auto id = ann.first;
        if(listPos->getID() == id) {
            auto frame = ann.second->frame+1;
            auto area = ann.second->area;
            auto loc = document->addAnnotationLocation(ann.first,frame, area);
            //auto rect = QRectF(area.x,area.y,area.w,area.h);
            //auto region = new AnnotatedRegion(id, loc, rect);
            //scene->addItem(region);
            //currentAnnotations.push_back(region);
            break;
        }
    }
    processAnnotations(player->getCurrentFrame());
}
/* Saving to turn into next and copy all
void MainWindow::on_nextAndCopy_clicked()
{
    // remove old annotations
    for (auto ann : currentAnnotations) {
        scene->removeItem(ann);
    }
    currentAnnotations.clear();
    auto prevFrame = player->getCurrentFrame();
    updateImage(player->nextFrame());
    //Currently delete existing annotations and overwrites. Want to change
    //to don't overwrite existing annotations.  Probably going to have to
    //do an exhaustive search of existing annotations for each potential
    //copied annotation.  Research code templates to shortcut this.  Make
    //sure to understand data types involved.

    //check this frame for annotations. We're going to delete anything
    //that currently exists.
    for (auto ann: document->getAnnotations()) {
        auto id = ann.first;
        removeRegion(id, prevFrame+1);
    }
    // copy annotations
    for (auto ann : document->getAnnotations(prevFrame)) {
        auto frame = ann.second->frame+1;
        auto area = ann.second->area;
        auto loc = document->addAnnotationLocation(ann.first,frame, area);
        auto rect = QRectF(area.x,area.y,area.w,area.h);
        auto region = new AnnotatedRegion(ann.first, loc, rect);
        scene->addItem(region);
        currentAnnotations.push_back(region);
    }
}
*/
void MainWindow::addFish(FishTypeEnum fType)
{

	player->Stop();
	ui->Play->setText(tr("Play"));
	double currentFrame = player->getCurrentFrame();
    int id;
    if (myFishList.size() == 0) {
        id = 1;
        nextID = 2;
    }
    else {
        id = nextID;
        //std::cout<<"ID is: "<<id<<std::endl;
        nextID++;

    }
    unique_ptr<Fish> tempFish(new Fish(fType,currentFrame,id));
    tempFish->setFishSubType(0);

    if (myFishList.size() == 0) {
        myFishList.push_back(*tempFish);
        listPos = myFishList.end()-1;
    }
    else {
        vector<Fish>::iterator tempLoc = myFishList.begin();
        int tempIndex = 0;
        while(tempLoc->getFrameCounted() < currentFrame) {
            tempLoc++;
            if(tempLoc==myFishList.end()) break;
            tempIndex++;
            //std::cout<<tempIndex<<std::endl;
        }
        if (tempLoc == myFishList.end()) {
            //std::cout<<"Putting at end"<<std::endl;
            myFishList.push_back(*tempFish);
            listPos = myFishList.end()-1;
        }
        else {
            if (tempLoc == myFishList.begin()) {
                myFishList.insert(tempLoc, *tempFish);
                listPos = myFishList.begin();
            }
            else {
                myFishList.insert(myFishList.begin()+tempIndex, *tempFish);
                listPos = myFishList.begin()+tempIndex;
            }
        }
    }
    //myFishList.push_back(*tempFish);
    //listPos = myFishList.end()-1;
    //std::sort(myFishList.begin(),myFishList.end());
    ui->fishNumVal->setText(QString::number(id));
    ui->frameCountedVal->setText(QString::number(currentFrame));
    ui->totalFishVal->setText(QString::number(myFishList.size()));
    ui->typeMenu->setCurrentIndex((int) fType);
    ui->subTypeMenu->setCurrentIndex((int) 0);
}

void MainWindow::updateVecIndex()
{
    ui->fishNumVal->setText(QString::number(listPos->getID()));
    ui->frameCountedVal->setText(QString::number(listPos->getFrameCounted()));
}

FishTypeEnum MainWindow::getFishType (string const& inString)
{
    if (inString == "ROUND") return ROUND;
    if (inString == "FLAT") return FLAT;
    if (inString == "SKATE") return SKATE;
    if (inString == "OTHER") return OTHER;
    return OTHER;
}

int MainWindow::getFishSpecies (FishTypeEnum fType, string const& sString)
{
    switch (fType)
    {
    case ROUND:
        if (sString == "Round") return 0;
        if (sString == "Haddock") return 1;
        if (sString == "Cod") return 2;
        if (sString == "Whiting") return 3;
        if (sString == "Red Hake") return 4;
        if (sString == "Pollock") return 5;
        if (sString == "Herring") return 6;
        if (sString == "Unknown") return 7;
        return 0;
        break;
    case FLAT:
        if (sString == "Flat") return 0;
        if (sString == "Yellowtail") return 1;
        if (sString == "Windowpane") return 2;
        if (sString == "Summer") return 3;
        if (sString == "FourSport") return 4;
        if (sString == "Grey Sole") return 5;
        if (sString == "Halibut") return 6;
        if (sString == "Unknown") return 7;
        return 0;
        break;
    case SKATE:
        if (sString == "Skate") return 0;
        if (sString == "Barndoor") return 1;
        if (sString == "Unknown") return 2;
        return 0;
        break;
    case OTHER:
        if (sString == "Other") return 0;
        if (sString == "Dogfish") return 1;
        if (sString == "Monkfish") return 2;
        if (sString == "Lobster") return 3;
        if (sString == "Scallop") return 4;
        return 0;
        break;
    default:
        return 0;
        break;
    }
}

string MainWindow::getFishTypeString (FishTypeEnum fType)
{
    switch (fType){
    case ROUND:
        return "ROUND";
        break;
    case FLAT:
        return "FLAT";
        break;
    case SKATE:
        return "SKATE";
        break;
    case OTHER:
        return "OTHER";
        break;
    default:
        return "OTHER";
        break;
    }
}

string MainWindow::getFishSpeciesString (FishTypeEnum fType, int species)
{
    switch (fType)
    {
    case ROUND:
        if (species == 0) return "Round";
        if (species == 1) return "Haddock";
        if (species == 2) return "Cod";
        if (species == 3) return "Whiting";
        if (species == 4) return "Red Hake";
        if (species == 5) return "Pollock";
        if (species == 6) return "Herring";
        if (species == 7) return "Unknown";
        return "Unknown";
        break;
    case FLAT:
        if (species == 0) return "Flat";
        if (species == 1) return "Yellowtail";
        if (species == 2) return "Windowpane";
        if (species == 3) return "Summer";
        if (species == 4) return "FourSport";
        if (species == 5) return "Grey Sole";
        if (species == 6) return "Halibut";
        if (species == 7) return "Unknown";
        return "Unknown";
        break;
    case SKATE:
        if (species == 0) return "Skate";
        if (species == 1) return "Barndoor";
        if (species == 2) return "Unknown";
        return "Unknown";
        break;
    case OTHER:
        if (species == 0) return "Other";
        if (species == 1) return "Dogfish";
        if (species == 2) return "Monkfish";
        if (species == 3) return "Lobster";
        if (species == 4) return "Scallop";
        return "Unknown";
        break;
    default:
        return "Unknown";
        break;
    }
}

void MainWindow::disableControls()
{
    ui->Play->setEnabled(false);
    ui->videoSlider->setEnabled(false);
    ui->SlowDown->setEnabled(false);
    ui->SpeedUp->setEnabled(false);
    ui->minusOneFrame->setEnabled(false);
    ui->plusOneFrame->setEnabled(false);
    ui->removeFish->setEnabled(false);
    ui->goToFrame->setEnabled(false);
    ui->addRound->setEnabled(false);
    ui->addFlat->setEnabled(false);
    ui->addSkate->setEnabled(false);
    ui->addOther->setEnabled(false);
    ui->saveAnnotate->setEnabled(false);
    ui->removeFish->setEnabled(false);
    ui->goToFrame->setEnabled(false);
    ui->prevFish->setEnabled(false);
    ui->nextFish->setEnabled(false);
    ui->loadAnnotate->setEnabled(false);
    ui->navigator->findChild<QPushButton *>("next_button")->setEnabled(false);
    ui->navigator->findChild<QPushButton *>("prev_button")->setEnabled(false);
    ui->navigator->findChild<QPushButton *>("add_region_button")->setEnabled(false);
    ui->navigator->findChild<QPushButton *>("remove_region_button")->setEnabled(false);
    ui->navigator->findChild<QPushButton *>("next_with_copy_button")->setEnabled(false);
}

void MainWindow::enableControls()
{
    ui->Play->setEnabled(true);
    ui->videoSlider->setEnabled(true);
    ui->SlowDown->setEnabled(true);
    ui->SpeedUp->setEnabled(true);
    ui->minusOneFrame->setEnabled(true);
    ui->plusOneFrame->setEnabled(true);
    ui->removeFish->setEnabled(true);
    ui->goToFrame->setEnabled(true);
    ui->addRound->setEnabled(true);
    ui->addFlat->setEnabled(true);
    ui->addSkate->setEnabled(true);
    ui->addOther->setEnabled(true);
    ui->saveAnnotate->setEnabled(true);
    ui->removeFish->setEnabled(true);
    ui->goToFrame->setEnabled(true);
    ui->prevFish->setEnabled(true);
    ui->nextFish->setEnabled(true);
    ui->loadAnnotate->setEnabled(true);
    ui->navigator->findChild<QPushButton *>("next_button")->setEnabled(true);
    ui->navigator->findChild<QPushButton *>("prev_button")->setEnabled(true);
    ui->navigator->findChild<QPushButton *>("add_region_button")->setEnabled(true);
    ui->navigator->findChild<QPushButton *>("remove_region_button")->setEnabled(true);
    ui->navigator->findChild<QPushButton *>("next_with_copy_button")->setEnabled(true);
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow* w = new MainWindow();
	w->setAttribute(Qt::WA_DeleteOnClose, true);

	w->show();

	return a.exec();
}
