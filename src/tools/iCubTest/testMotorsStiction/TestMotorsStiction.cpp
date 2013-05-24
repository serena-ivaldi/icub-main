// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* 
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include "TestMotorsStiction.h"
#include  <yarp/os/Time.h>
#include  <gsl/gsl_math.h>

iCubTestMotorsStiction::iCubTestMotorsStiction(yarp::os::Searchable& configuration) : iCubTest(configuration)
{
    m_aHomePos=NULL;
    m_aHomeVel=NULL;
    m_aTolerance=NULL;
    m_aPWM=NULL;
    m_aTimeout=NULL;

    m_Part=(iCubDriver::iCubPart)0;

    if (configuration.check("device"))
    {
        std::string device(configuration.find("device").asString());

        for (int p=0; p<iCubDriver::NUM_ICUB_PARTS; ++p)
        {
            if (device==iCubDriver::m_aiCubPartName[p])
            {
                m_Part=(iCubDriver::iCubPart)p;
                break;
            }
        }
    }

    m_NumJoints=iCubDriver::instance()->getNumOfJoints(m_Part);

    ///////////////////////////////////////////////////////////////
    /*
    if (m_nJoints<=0)
    {
    printf("\nERROR\n\n");
    return;
    }
    */
    ///////////////////////////////////////////////////////////////

    if (configuration.check("homePosition"))
    {
        yarp::os::Bottle bot=configuration.findGroup("homePosition").tail();

        int n=m_NumJoints<bot.size()?m_NumJoints:bot.size();

        m_aHomePos=new double[m_NumJoints];

        for (int i=0; i<n; ++i)
        {
            if (bot.get(i).isDouble()) m_aHomePos[i]=bot.get(i).asDouble();
        }
    }

    if (configuration.check("homeVelocity"))
    {
        yarp::os::Bottle bot=configuration.findGroup("homeVelocity").tail();

        int n=m_NumJoints<bot.size()?m_NumJoints:bot.size();

        m_aHomeVel=new double[m_NumJoints];

        for (int i=0; i<n; ++i)
        {
            if (bot.get(i).isDouble()) m_aHomeVel[i]=bot.get(i).asDouble();
        }
    }

    if (configuration.check("tolerance"))
    {
        yarp::os::Bottle bot=configuration.findGroup("Tolerance").tail();

        int n=m_NumJoints<bot.size()?m_NumJoints:bot.size();

        m_aTolerance=new double[m_NumJoints];

        for (int i=0; i<n; ++i)
        {
            if (bot.get(i).isDouble()) m_aTolerance[i]=bot.get(i).asDouble();
        }
    }

    if (configuration.check("PWM"))
    {
        yarp::os::Bottle bot=configuration.findGroup("PWM").tail();

        int n=m_NumJoints<bot.size()?m_NumJoints:bot.size();

        m_aPWM=new double[m_NumJoints];

        for (int i=0; i<n; ++i)
        {
            if (bot.get(i).isDouble()) m_aPWM[i]=bot.get(i).asDouble();
        }
    }

    if (configuration.check("timeout"))
    {
        yarp::os::Bottle bot=configuration.findGroup("timeout").tail();

        int n=m_NumJoints<bot.size()?m_NumJoints:bot.size();

        m_aTimeout=new double[m_NumJoints];

        for (int i=0; i<n; ++i)
        {
            if (bot.get(i).isDouble()) m_aTimeout[i]=bot.get(i).asDouble();
        }
    }
}

iCubTestMotorsStiction::~iCubTestMotorsStiction()
{
    if (m_aHomePos)   delete [] m_aHomePos;
    if (m_aHomeVel)   delete [] m_aHomeVel;
    if (m_aTolerance) delete [] m_aTolerance;
    if (m_aPWM)       delete [] m_aPWM;
    if (m_aTimeout)   delete [] m_aTimeout;
}

iCubTestReport* iCubTestMotorsStiction::run()
{
    iCubTestReport* pTestReport=new iCubTestReport(m_Name,m_PartCode,m_Description);

    m_bSuccess=true;

    for (int joint=0; joint<m_NumJoints; ++joint)
    {
        iCubTestMotorsStictionReportEntry *pOutput=new iCubTestMotorsStictionReportEntry();

        char jointName[8];
        char tmpString[200];

        pOutput->m_Name="Joint ";
        sprintf(jointName,"%d",joint);
        pOutput->m_Name+=jointName;
        pOutput->m_Name+=" position";
        
        sprintf(tmpString,"%f",m_aPWM[joint]);
        pOutput->m_PWM=std::string(tmpString);
        sprintf(tmpString,"%f",m_aTimeout[joint]);
        pOutput->m_Timeout=std::string(tmpString);
        sprintf(tmpString,"%f",m_aTolerance[joint]);
        pOutput->m_Tolerance=std::string(tmpString);

        double joint_min = 0.0;
        double joint_max = 0.0;
        iCubDriver::instance()->getJointLimits(m_Part,joint,joint_min, joint_max);
        sprintf(tmpString,"%f",joint_min);
        pOutput->m_MinLim=std::string(tmpString);
        sprintf(tmpString,"%f",joint_max);
        pOutput->m_MaxLim=std::string(tmpString);

        double posPidSign = 1.0;
        iCubDriver::instance()->getPosPidSign(m_Part,joint,posPidSign);
        if (posPidSign<0) m_aPWM[joint] = -m_aPWM[joint];

        iCubDriver::ResultCode result;
        result=iCubDriver::instance()->setPos(m_Part,joint,m_aHomePos[joint],m_aHomeVel[joint],0.0);
        yarp::os::Time::delay(m_aTimeout[joint]);

        double joint_pos1 = 0.0;
        result=iCubDriver::instance()->startOpenloopCmd(m_Part,joint,m_aPWM[joint]);
        yarp::os::Time::delay(m_aTimeout[joint]);
        result=iCubDriver::instance()->getEncPos(m_Part,joint,joint_pos1);
        result=iCubDriver::instance()->stopOpenloopCmd(m_Part,joint);

        result=iCubDriver::instance()->setPos(m_Part,joint,m_aHomePos[joint],m_aHomeVel[joint],0.0);
        yarp::os::Time::delay(m_aTimeout[joint]);

        double joint_pos2 = 0.0;
        result=iCubDriver::instance()->startOpenloopCmd(m_Part,joint,-m_aPWM[joint]);
        yarp::os::Time::delay(m_aTimeout[joint]);
        result=iCubDriver::instance()->getEncPos(m_Part,joint,joint_pos2);
        result=iCubDriver::instance()->stopOpenloopCmd(m_Part,joint);

        result=iCubDriver::instance()->setPos(m_Part,joint,m_aHomePos[joint],m_aHomeVel[joint],0.0);
        yarp::os::Time::delay(m_aTimeout[joint]);

        if (fabs(joint_pos1-joint_max)>m_aTolerance[joint] ||
            fabs(joint_pos2-joint_min)>m_aTolerance[joint])
        { 
            pOutput->m_Result="FAILED: unable to reach limits";
            m_bSuccess=false;
            pTestReport->incFailures();
        }
        else 
        {
            pOutput->m_Result="SUCCESS";
        }

        sprintf(tmpString,"%f",joint_pos1);
        pOutput->m_MinLimReached=std::string(tmpString);

        sprintf(tmpString,"%f",joint_pos2);
        pOutput->m_MaxLimReached=std::string(tmpString);

        /****
        iCubDriver::ResultCode result = iCubDriver::IPOS_POSMOVE_OK;
        //result=iCubDriver::instance()->setPos(m_Part,joint,m_aTargetVal[joint],m_aRefVel?m_aRefVel[joint]:0.0,m_aRefAcc?m_aRefAcc[joint]:0.0);

        bool bSetPosSuccess=false;

        switch (result)
        {
        case iCubDriver::DRIVER_FAILED:
            pOutput->m_Result="FAILED: !PolyDriver";
            break;
        case iCubDriver::IPOS_FAILED:
            pOutput->m_Result="FAILED: !IPositionControl";
            break;
        case iCubDriver::IPOS_POSMOVE_FAILED:
            pOutput->m_Result="FAILED: IPositionControl->positionMove";
            break;
        case iCubDriver::IPOS_SETREFSPEED_FAILED:
            pOutput->m_Result="FAILED: IPositionControl->setRefSpeed";
            break;
        case iCubDriver::IPOS_SETREFACC_FAILED:
            pOutput->m_Result="FAILED: IPositionControl->setRefAcceleration";
            break;
        case iCubDriver::IPOS_POSMOVE_OK:
            bSetPosSuccess=true;
        }

        if (!bSetPosSuccess)
        {
            m_bSuccess=false;
            pTestReport->incFailures();
            pTestReport->addEntry(pOutput);
            continue;
        }

        // only if success

        result=iCubDriver::instance()->waitPos(m_Part,joint,m_aTimeout[joint]);
        bool bWaitPosSuccess=false;
        switch (result)
        {
        case iCubDriver::IPOS_FAILED:
            pOutput->m_Result="FAILED: !IPositionControl";
            break;
        case iCubDriver::IPOS_CHECKMOTIONDONE_FAILED:
            pOutput->m_Result="FAILED: IPositionControl->checkMotionDone";
            break;
        case iCubDriver::IPOS_CHECKMOTIONDONE_TIMEOUT:
            pOutput->m_Result="FAILED: Timeout in IPositionControl->positionMove";
            break;
        case iCubDriver::IPOS_CHECKMOTIONDONE_OK:
            bWaitPosSuccess=true;
        }

        // { encoders 
        double pos;
        result=iCubDriver::instance()->getEncPos(m_Part,joint,pos);
        bool bGetEncPosSuccess=false;
        switch (result)
        {
        case iCubDriver::IENC_FAILED:
            if (bWaitPosSuccess)
            {
                pOutput->m_Result="FAILED: !IEncoders";
            }
            break;
        case iCubDriver::IENC_GETPOS_FAILED:
            if (bWaitPosSuccess)
            {
                pOutput->m_Result="FAILED: IEncoders->getEncoder";
            }
            break;
        case iCubDriver::IENC_GETPOS_OK:
            //sprintf(posString,"%f",pos);
            //pOutput->m_Value=posString;
            bGetEncPosSuccess=true;
        }

        if (!bWaitPosSuccess || !bGetEncPosSuccess)
        {
            m_bSuccess=false;
            pTestReport->incFailures();
            pTestReport->addEntry(pOutput);
            continue;
        }

        if (pos>=m_aTargetVal[joint]+m_aMinErr[joint] && pos<=m_aTargetVal[joint]+m_aMaxErr[joint])
        {
            pOutput->m_Result="SUCCESS";
        }
        else
        {
            m_bSuccess=false;
            pTestReport->incFailures();
            pOutput->m_Result="FAILED: value out of range";
        }
*****/

        pTestReport->addEntry(pOutput);
    }

    return pTestReport;
}