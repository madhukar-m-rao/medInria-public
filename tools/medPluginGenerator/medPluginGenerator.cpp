/* medPluginGenerator.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Mar  9 21:41:18 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 11 14:51:33 2011 (+0100)
 *           By: Julien Wintz
 *     Update #: 176
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medPluginGenerator.h"

class medPluginGeneratorPrivate
{
public:
    QString plugin;
    medPluginGenerator::PluginFamily pluginFamily;
    QString pluginFamilyString;
    QString output;
    QString namesp;
    QString name;
    QString type;
    QString description;
    QString license;
    
    QDir parent;
    QDir target;
};


medPluginGenerator::medPluginGenerator(void)
{
    this->d = new medPluginGeneratorPrivate;
}

medPluginGenerator::~medPluginGenerator(void)
{
    delete this->d;
    this->d = NULL;
}

void medPluginGenerator::setPluginFamily(const medPluginGenerator::PluginFamily family)
{
    d->pluginFamily = family;
    switch (family)
    {
        case medPluginGenerator::FILTERING:
            d->pluginFamilyString = "filtering";
            break;
        case medPluginGenerator::REGISTRATION:
            d->pluginFamilyString = "registration";
            break;
        case medPluginGenerator::GENERIC:
        default:
            d->pluginFamilyString = "generic";
            break;
    }
    
    
}

void medPluginGenerator::setOutputDirectory(const QString& directory)
{
    d->output = directory;
}

void medPluginGenerator::setNamespace(const QString& namesp)
{
    d->namesp = namesp;
    d->namesp.replace(0, 1, QString(d->namesp).left(1).toLower());
}

void medPluginGenerator::setName(const QString& name)
{
    d->name = name;
}

void medPluginGenerator::setType(const QString& type)
{
    d->type = type;
}

void medPluginGenerator::setDescription(const QString& desc)
{
    d->description = desc;
}

void medPluginGenerator::setLicense(const QString& license)
{
    d->license = license;
}

bool medPluginGenerator::run(void)
{
    d->parent = QDir(d->output);
    
    if(!d->parent.exists()) {
        qWarning() << "medPluginGenerator: parent directory is not valid.";
        return false;
    }
    
    d->plugin = QString("%1").arg(d->name.replace(0, 1, QString(d->name).left(1).toLower()));
    
    if(!d->parent.mkdir(QString(d->plugin))) {
        qWarning() << "medPluginGenerator: unable to create target directory.";
        return false;
    }
    
    d->target = QDir(d->parent);
    
    if(!d->target.cd(QString(d->plugin))) {
        qWarning() << "medPluginGenerator: unable to move to target directory.";
        return false;
    }
    
    if (d->pluginFamily != GENERIC)
        return generateCMakeLists()
        && generateTypeHeaderFile()
        && generateTypeSourceFile()
        && generateTypeToolBoxSourceFile()
        && generateTypeToolBoxHeaderFile()
        && generatePluginHeaderFile()
        && generatePluginSourceFile()
        && generateExportHeaderFile()
        && generateHelpCollectionFile()
        && generateHelpConfigurationFile()
        && generateReadmeFile()
        && generateCopyingFile();
    
    return generateCMakeLists()
    && generateTypeHeaderFile()
    && generateTypeSourceFile()
    && generatePluginHeaderFile()
    && generatePluginSourceFile()
    && generateExportHeaderFile()
    && generateHelpCollectionFile()
    && generateHelpConfigurationFile()
    && generateReadmeFile()
    && generateCopyingFile();
}

// /////////////////////////////////////////////////////////////////
// CMakeLists.txt
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateCMakeLists(void)
{
    QFile targetFile(d->target.absoluteFilePath("CMakeLists.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open CMakeLists.txt for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/cmake").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll()).arg(QString(d->plugin));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeHeaderFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append(".h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append(".h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/type.h").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin))
	.arg(QString(d->plugin).toUpper())
	.arg(QString(d->type))
	.arg(QString(d->plugin).replace(0, 1, QString(d->plugin).left(1).toUpper()))
    .arg(QString(d->namesp));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeSourceFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append(".cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append(".cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/type.cpp").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin))
    .arg(QString(d->type))
    .arg(QString(d->plugin).replace(0, 1, QString(d->plugin).left(1).toUpper()))
    .arg(QString(d->namesp))
    .arg(QString(d->name).replace(0, 1, QString(d->name).left(1).toUpper()));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type ToolBox header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeToolBoxHeaderFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("ToolBox.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("ToolBox.h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/typeToolBox.h").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin))
    .arg(QString(d->plugin).toUpper())
    .arg(QString(d->plugin).replace(0, 1, QString(d->plugin).left(1).toUpper()))
    .arg(QString(d->namesp));
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Type ToolBox source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateTypeToolBoxSourceFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("ToolBox.cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("ToolBox.cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/typeToolBox.cpp").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin))
    .arg(d->name)
    .arg(QString(d->plugin).replace(0, 1, QString(d->plugin).left(1).toUpper()))
    .arg(QString(d->namesp));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}


// /////////////////////////////////////////////////////////////////
// Plugin header file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generatePluginHeaderFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.h") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/plugin.h").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll())
    .arg(QString(d->plugin))
    .arg(QString(d->plugin).toUpper())
    .arg(QString(d->namesp));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Plugin source file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generatePluginSourceFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.cpp")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.cpp") << "for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/%1/plugin.cpp").arg(d->pluginFamilyString));
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll()).arg(QString(d->plugin))
    .arg(QString(d->plugin).toUpper())
    .arg(QString(d->description))
    .arg(QString(d->namesp));
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Export file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateExportHeaderFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("PluginExport.h")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("PluginExport.h") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/export.h");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll()).arg(QString(d->plugin)).arg(QString(d->plugin).toUpper());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// Help collection file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateHelpCollectionFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.qhcp.in")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.qhcp.in") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/qhcp");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}


// /////////////////////////////////////////////////////////////////
// Help configuration file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateHelpConfigurationFile(void)
{
    QFile targetFile(d->target.absoluteFilePath(QString(d->plugin).append("Plugin.doxyfile.in")));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open" << QString(d->plugin).append("Plugin.doxyfile.in") << "for writing";
        return false;
    }
    
    QFile templateFile(":template/doxyfile");
    
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    templateFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// README file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateReadmeFile(void)
{
    QFile targetFile(d->target.absoluteFilePath("README.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open CMakeLists.txt for writing";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << d->description;
    
    targetFile.close();
    
    return true;
}

// /////////////////////////////////////////////////////////////////
// COPYING file
// /////////////////////////////////////////////////////////////////

bool medPluginGenerator::generateCopyingFile(void)
{
    QFile targetFile(d->target.absoluteFilePath("COPYING.txt"));
    
    if(!targetFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "medPluginGenerator: unable to open COPYING.txt for writing";
        return false;
    }
    
    QFile templateFile(QString(":template/license/").append(d->license));
    if(!templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "medPluginGenerator: unable to open template file " << templateFile.fileName() << " for reading";
        return false;
    }
    
    QTextStream stream(&targetFile);
    
    stream << QString(templateFile.readAll());
    
    targetFile.close();
    
    return true;
}

