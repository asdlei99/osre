﻿/*-----------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2015-2019 OSRE ( Open Source Render Engine ) by Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-----------------------------------------------------------------------------------------------*/
using OSREEditor.View;
using System;
using System.Windows.Forms;

namespace OSREEditor.Model.Actions
{
    /// <summary>
    /// This class implements the new project action.
    /// </summary>
    public class NewProjectAction : IAction
    {
        /// <summary>
        /// The project name property access.
        /// </summary>
        public string ProjectName { get; set; }

        private IntPtr mHandle;

        private Form mMainWindow;

        /// <summary>
        /// The current project property access.
        /// </summary>
        public Project CurrentProject { get; set; }

        /// <summary>
        /// The class constructor with all parameters.
        /// </summary>
        /// <param name="handle">The window handle</param>
        /// <param name="mainWindow">The main window instance</param>
        public NewProjectAction(IntPtr handle, Form mainWindow)
        {
            mHandle = handle;
            mMainWindow = mainWindow;
        }

        /// <summary>
        /// Will create the new project.
        /// </summary>
        /// <returns>true if successful, false in case of an error.</returns>
        public bool Execute()
        {
            if (mHandle == null)
            {
                return false;
            }

            if (ProjectName.Length == 0)
            {
                ProjectName = "New Project";
            }
            if (mMainWindow != null)
            {
                mMainWindow.Text = ProjectName;
            }
            CurrentProject = new Project
            {
                ProjectName = ProjectName
            };
            
            int retCode = OSREWrapper.CreateEditorApp( mHandle );
            OSREWrapper.NewProject(CurrentProject.ProjectName);

            return 0 == retCode;
        }
    }
}
