/****************************************************************************
** Copyright (c) 2017 Adrian Schneider, AOT AG
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
*****************************************************************************/

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>


#include "vtkMeshVisualizer.h"

void VTKMeshVisualizer::displayMesh( const vtkSmartPointer<vtkPolyData>& mesh )
{
    // show mesh
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<VTKMeshVisualizerInteraction> pickingInteraction = vtkSmartPointer<VTKMeshVisualizerInteraction>::New();
    pickingInteraction->SetDefaultRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle( pickingInteraction );
    renderWindowInteractor->SetRenderWindow( renderWindow );

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData( mesh );

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    renderWindow->Render();
    renderWindowInteractor->Start();

}



VTKMeshVisualizerInteraction* VTKMeshVisualizerInteraction::New()
{
    VTKMeshVisualizerInteraction* result = new VTKMeshVisualizerInteraction();
    vtkObjectFactory::ConstructInstance(result->GetClassName());
    return result;
}

VTKMeshVisualizerInteraction::VTKMeshVisualizerInteraction()
{
}

void VTKMeshVisualizerInteraction::initIfNecessary()
{
    // init at first time
    if( m_pickingMarker == NULL )
    {
        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius( 0.4 );

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection( sphereSource->GetOutputPort() );

        m_pickingMarker = vtkSmartPointer<vtkActor>::New();
        m_pickingMarker->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
        m_pickingMarker->SetMapper( mapper );

        this->GetDefaultRenderer()->AddActor( m_pickingMarker );

        m_lastMouseClick = std::chrono::system_clock::now();
    }
}

// code based on example http://www.vtk.org/Wiki/VTK/Examples/Cxx/Interaction/Picking
void VTKMeshVisualizerInteraction::OnLeftButtonDown()
{
    initIfNecessary();

    if( isDoubleClick() )
    {
        // mouse click position
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // pick surface in 3d scene
        vtkSmartPointer<vtkPropPicker>  picker = vtkSmartPointer<vtkPropPicker>::New();
        picker->Pick( clickPos[0], clickPos[1], 0, this->GetDefaultRenderer() );

        double* pos = picker->GetPickPosition();
        std::cout << "Pick position: [ " << pos[0] << "; " << pos[1] << "; " << pos[2] << " ] "<< std::endl;

        m_pickingMarker->SetPosition( pos );
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

bool VTKMeshVisualizerInteraction::isDoubleClick()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<double> timeBetweenClicks = now - m_lastMouseClick;
    m_lastMouseClick = now;
    return timeBetweenClicks.count() < 0.5; // 0.5 seconds
}
