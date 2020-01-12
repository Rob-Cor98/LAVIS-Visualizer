/*
* Image visualization accepting the following image format:
*		vtp, ply, obj, vtk, vtu, pdb, stl
* 
* Use windows forms, vtk library, openvr sdk and boost library
*/


/*
*		VTK C++ CODE
*/

// VTK includes
#include <vtkActor.h>
#include <vtkAppendFilter.h>
#include <vtkBYUReader.h>
#include <vtkCamera.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>
#include <vtkDICOMImageReader.h>
#include <vtkDirectory.h>
#include <vtkExtractEdges.h>
#include <vtkGlyph3D.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLODActor.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNamedColors.h>
#include <vtkOBJReader.h>
#include <vtkOpenVRCamera.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkPDBReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSphereSource.h>
#include <vtkSTLReader.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkVolumeProperty.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtksys/SystemTools.hxx>

// Other includes
#include <cmath>
#include <msclr\marshal_cppstd.h>
#include <boost/algorithm/string/replace.hpp>


// C++ function for VTK displaying 3D models
void VTKdisplay(std::string filename, double scale) {
	if (scale == NULL || scale == 0) { // if scale checkbox = false, set scale to 1
		scale = 1;
	}

	std::string directoryName = filename;
	vtkSmartPointer<vtkDirectory> directory =
		vtkSmartPointer<vtkDirectory>::New();
	int opened = directory->Open(directoryName.c_str());

	// VR Renderer & RendererWindow
	/*vtkSmartPointer<vtkOpenVRRenderer> renderer =
		vtkSmartPointer<vtkOpenVRRenderer>::New();
	vtkSmartPointer<vtkOpenVRRenderWindow> renWin =
		vtkSmartPointer<vtkOpenVRRenderWindow>::New();
	renWin->AddRenderer(renderer);
	vtkSmartPointer<vtkOpenVRRenderWindowInteractor> renWinInteractor =
		vtkSmartPointer<vtkOpenVRRenderWindowInteractor>::New();
	vtkNew<vtkOpenVRCamera> cam;*/
	// End VR Renderer & RendererWindow

	// Desktop Renderer & RendererWindow
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkNew<vtkCamera> cam;
	// End Desktop Renderer & RendererWindow

	vtkSmartPointer<vtkMultiBlockDataSet> root =
		vtkSmartPointer<vtkMultiBlockDataSet>::New();
	vtkSmartPointer<vtkMultiBlockDataSet> branch =
		vtkSmartPointer<vtkMultiBlockDataSet>::New();
	root->SetBlock(0, branch);

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();

	vtkSmartPointer<vtkCompositeDataGeometryFilter> polydata =
		vtkSmartPointer<vtkCompositeDataGeometryFilter>::New();

	std::string ext = "";
	int numberOfFiles = directory->GetNumberOfFiles();
	bool checked = false;

	for (int i = 0; i < numberOfFiles; i++) {
		std::string fileString = directoryName;
		fileString += directory->GetFile(i);
		ext = vtksys::SystemTools::GetFilenameLastExtension(fileString);
		const char* cstr = fileString.c_str();

		if (ext == ".vtp") {
			vtkSmartPointer<vtkXMLPolyDataReader> reader =
				vtkSmartPointer<vtkXMLPolyDataReader>::New();
			reader->SetFileName(cstr);
			reader->Update();
			root->SetBlock(i, reader->GetOutput());

			vtkSmartPointer<vtkTransform> transform =
				vtkSmartPointer<vtkTransform>::New();
			transform->Scale(.005, .005, .005);

			vtkSmartPointer<vtkTransformFilter> transformFilter =
				vtkSmartPointer<vtkTransformFilter>::New();
			transformFilter->SetInputData(root);
			transformFilter->SetTransform(transform);

			polydata->SetInputConnection(transformFilter->GetOutputPort());

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(0, polydata->GetOutputPort(0));
			
			actor->SetScale(scale, scale, scale);
			actor->SetPosition(-0.7, 0.3, -2.5);
			actor->SetMapper(mapper);
			renderer->AddActor(actor);
		}

		if (ext == ".ply") {
			vtkSmartPointer<vtkPLYReader> reader =
				vtkSmartPointer<vtkPLYReader>::New();
			reader->SetFileName(cstr);
			reader->Update();
			root->SetBlock(i, reader->GetOutput());

			vtkSmartPointer<vtkTransform> transform =
				vtkSmartPointer<vtkTransform>::New();
			transform->Scale(.005, .005, .005);

			vtkSmartPointer<vtkTransformFilter> transformFilter =
				vtkSmartPointer<vtkTransformFilter>::New();
			transformFilter->SetInputData(root);
			transformFilter->SetTransform(transform);

			polydata->SetInputConnection(transformFilter->GetOutputPort());

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(0, polydata->GetOutputPort(0));

			actor->SetPosition(-0.5, 0.5, -2.5);
			actor->SetScale(scale, scale, scale);
			actor->SetMapper(mapper);
			renderer->AddActor(actor);
		}

		if (ext == ".obj") {
			vtkSmartPointer<vtkOBJReader> reader =
				vtkSmartPointer<vtkOBJReader>::New();
			reader->SetFileName(cstr);
			reader->Update();

			vtkSmartPointer<vtkNamedColors> colors =
				vtkSmartPointer<vtkNamedColors>::New();
			vtkColor3d backgroundColor = colors->GetColor3d("SpringGreen");
			vtkColor3d actorColor = colors->GetColor3d("HoneyDew");

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(reader->GetOutputPort());

			actor->SetPosition(-1.5, 0.2, -5);
			actor->SetScale(scale, scale, scale);
			actor->SetMapper(mapper);
			actor->GetProperty()->SetDiffuseColor(actorColor.GetData());

			renderer->AddActor(actor);
			renderer->GetActiveCamera()->Azimuth(30);
			renderer->GetActiveCamera()->Elevation(30);
			renderer->GetActiveCamera()->Dolly(1.5);
			renderer->SetBackground(backgroundColor.GetData());
		}

		if (ext == ".stl") {
			vtkSmartPointer<vtkSTLReader> reader =
				vtkSmartPointer<vtkSTLReader>::New();
			reader->SetFileName(cstr);
			reader->Update();

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(reader->GetOutputPort());

			actor->SetScale(scale, scale, scale);
			actor->SetPosition(-1.5, 0.2, -5);
			actor->SetMapper(mapper);
			renWin->AddRenderer(renderer);
			renderer->AddActor(actor);
			renderer->SetBackground(.3, .6, .3);
		}

		if (ext == ".vtk") {
			vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid;
			vtkSmartPointer<vtkUnstructuredGrid> ReadUnstructuredGrid;

			vtkSmartPointer<vtkUnstructuredGridReader> reader =
				vtkSmartPointer<vtkUnstructuredGridReader>::New();
			reader->SetFileName(cstr);
			reader->Update();
			unstructuredGrid = reader->GetOutput();

			auto colors = vtkSmartPointer<vtkNamedColors>::New();

			renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
			renderer->UseHiddenLineRemovalOn();

			auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
			mapper->SetInputData(unstructuredGrid);
			mapper->ScalarVisibilityOff();

			auto backProp = vtkSmartPointer<vtkProperty>::New();
			backProp->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
			backProp->SetSpecular(.6);
			backProp->SetSpecularPower(30);

			actor->SetScale(scale, scale, scale);
			actor->SetPosition(-2, 0, -6);
			actor->SetMapper(mapper);
			actor->SetBackfaceProperty(backProp);
			actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Tomato").GetData());
			actor->GetProperty()->SetSpecular(.3);
			actor->GetProperty()->SetSpecularPower(30);

			renderer->AddActor(actor);
			renderer->GetActiveCamera()->Azimuth(45);
			renderer->GetActiveCamera()->Elevation(45);
		}

		if (ext == ".vtu") {
			vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid;
			vtkSmartPointer<vtkUnstructuredGrid> ReadUnstructuredGrid;

			vtkSmartPointer<vtkXMLUnstructuredGridReader> reader =
				vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
			reader->SetFileName(cstr);
			reader->Update();
			unstructuredGrid = reader->GetOutput();

			auto colors = vtkSmartPointer<vtkNamedColors>::New();

			renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
			renderer->UseHiddenLineRemovalOn();

			auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
			mapper->SetInputData(unstructuredGrid);
			mapper->ScalarVisibilityOff();

			auto backProp = vtkSmartPointer<vtkProperty>::New();
			backProp->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
			backProp->SetSpecular(.6);
			backProp->SetSpecularPower(30);

			actor->SetScale(scale, scale, scale);
			actor->SetOrigin(-4, 0, -10);
			actor->SetMapper(mapper);
			actor->SetBackfaceProperty(backProp);
			actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Tomato").GetData());
			actor->GetProperty()->SetSpecular(.3);
			actor->GetProperty()->SetSpecularPower(30);

			renderer->AddActor(actor);
			renderer->GetActiveCamera()->Azimuth(45);
			renderer->GetActiveCamera()->Elevation(45);
		}

		if (ext == ".pdb") {
			vtkSmartPointer<vtkNamedColors> colors =
				vtkSmartPointer<vtkNamedColors>::New();

			renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
			renWin->AddRenderer(renderer);

			vtkSmartPointer<vtkPDBReader> pdb =
				vtkSmartPointer<vtkPDBReader>::New();
			pdb->SetFileName(cstr);
			pdb->SetHBScale(1.0);
			pdb->SetBScale(1.0);
			pdb->Update();

			std::cout << "# of atoms is: " << pdb->GetNumberOfAtoms() << std::endl;

			double resolution = std::sqrt(300000.0 / pdb->GetNumberOfAtoms());
			if (resolution > 20)
			{
				resolution = 20;
			}
			if (resolution < 4)
			{
				resolution = 4;
			}
			std::cout << "Resolution is: " << resolution << std::endl;

			vtkSmartPointer<vtkSphereSource> sphere =
				vtkSmartPointer<vtkSphereSource>::New();
			sphere->SetCenter(0, 0, 0);
			sphere->SetRadius(1);
			sphere->SetThetaResolution(static_cast<int>(resolution));
			sphere->SetPhiResolution(static_cast<int>(resolution));

			vtkSmartPointer<vtkGlyph3D> glyph =
				vtkSmartPointer<vtkGlyph3D>::New();
			glyph->SetInputConnection(pdb->GetOutputPort());
			glyph->SetOrient(1);
			glyph->SetColorMode(1);
			glyph->ScalingOn();
			glyph->SetScaleMode(2);
			glyph->SetScaleFactor(.5);
			glyph->SetSourceConnection(sphere->GetOutputPort());

			vtkSmartPointer<vtkPolyDataMapper> atomMapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			atomMapper->SetInputConnection(glyph->GetOutputPort());
			atomMapper->UseLookupTableScalarRangeOff();
			atomMapper->ScalarVisibilityOn();
			atomMapper->SetScalarModeToDefault();

			vtkSmartPointer<vtkLODActor> atom =
				vtkSmartPointer<vtkLODActor>::New();
			atom->SetMapper(atomMapper);
			atom->GetProperty()->SetRepresentationToSurface();
			atom->GetProperty()->SetInterpolationToGouraud();
			atom->GetProperty()->SetAmbient(0.1);
			atom->GetProperty()->SetDiffuse(0.7);
			atom->GetProperty()->SetSpecular(0.5);
			atom->GetProperty()->SetSpecularPower(80);
			atom->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
			atom->SetNumberOfCloudPoints(30000);

			renderer->AddActor(atom);

			vtkSmartPointer<vtkTubeFilter> tube =
				vtkSmartPointer<vtkTubeFilter>::New();
			tube->SetInputConnection(pdb->GetOutputPort());
			tube->SetNumberOfSides(static_cast<int>(resolution));
			tube->CappingOff();
			tube->SetRadius(0.2);
			tube->SetVaryRadius(0);
			tube->SetRadiusFactor(10);

			vtkSmartPointer<vtkPolyDataMapper> bondMapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			bondMapper->SetInputConnection(tube->GetOutputPort());
			bondMapper->UseLookupTableScalarRangeOff();
			bondMapper->ScalarVisibilityOff();
			bondMapper->SetScalarModeToDefault();

			vtkSmartPointer<vtkLODActor> bond =
				vtkSmartPointer<vtkLODActor>::New();
			bond->SetMapper(bondMapper);
			bond->GetProperty()->SetRepresentationToSurface();
			bond->GetProperty()->SetInterpolationToGouraud();
			bond->GetProperty()->SetAmbient(0.1);
			bond->GetProperty()->SetDiffuse(0.7);
			bond->GetProperty()->SetSpecular(0.5);
			bond->GetProperty()->SetSpecularPower(80);
			bond->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
		}


		if (i == (numberOfFiles - 1)) {
			checked = true;
		}
	}

	if (ext != ".vtp" && ext != ".ply" && ext != ".obj" && ext != ".stl" && ext != ".vtk" && ext != ".vtu" && ext != ".pdb") {
		std::cout << "Formato de imagen incorrecto" << std::endl;
		checked = false;
	}
	else {
		checked = true;
	}
	

	if (checked == true) {
		// VR RenderWindowInteractor & Camera
		renWinInteractor->SetRenderWindow(renWin);
		// End VR RenderWindowInteractor & Camera

		renderer->SetActiveCamera(cam);
		renderer->SetBackground(.3, .6, .3);

		renWinInteractor->Start();
	}
}

/*
*		END VTK C++ CODE
*/


/*
*		WINDOWS FORM CODE
*/

namespace WindowsCDemo {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// Summary for MyForm
	public ref class MyForm : public System::Windows::Forms::Form
	{
		public:
			MyForm(void)
			{
				InitializeComponent();
			}

		protected:
			~MyForm()
			{
				if (components)
				{
					delete components;
				}
			}
		private: System::Windows::Forms::Label^ label1;
		private: System::Windows::Forms::Button^ button1;
		private: System::Windows::Forms::Label^ label2;

		private: System::Windows::Forms::NumericUpDown^ numericUpDown1;
	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::Label^ label3;

		protected:

		private:
			System::ComponentModel::Container ^components;

	#pragma region Windows Form Designer generated code
			/// Modify the contents of this method ONLY with the design editor/view.
			void InitializeComponent(void)
			{
				System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->button1 = (gcnew System::Windows::Forms::Button());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
				this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
				this->label3 = (gcnew System::Windows::Forms::Label());
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
				this->SuspendLayout();
				// 
				// label1
				// 
				this->label1->AutoSize = true;
				this->label1->Font = (gcnew System::Drawing::Font(L"Arial", 9.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(0)));
				this->label1->Location = System::Drawing::Point(13, 14);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(531, 108);
				this->label1->TabIndex = 0;
				this->label1->Text = resources->GetString(L"label1.Text");
				this->label1->Click += gcnew System::EventHandler(this, &MyForm::label1_Click);
				// 
				// button1
				// 
				this->button1->Font = (gcnew System::Drawing::Font(L"Arial", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(0)));
				this->button1->Location = System::Drawing::Point(354, 260);
				this->button1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
				this->button1->Name = L"button1";
				this->button1->Size = System::Drawing::Size(221, 46);
				this->button1->TabIndex = 1;
				this->button1->Text = L"Seleccionar Archivo";
				this->button1->UseVisualStyleBackColor = true;
				this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
				// 
				// label2
				// 
				this->label2->AutoSize = true;
				this->label2->Location = System::Drawing::Point(13, 277);
				this->label2->Margin = System::Windows::Forms::Padding(5, 0, 5, 0);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(226, 34);
				this->label2->TabIndex = 2;
				this->label2->Text = L"Si no se selecciona escala o es 0, \r\nse deja la escala original. ";
				this->label2->Click += gcnew System::EventHandler(this, &MyForm::label2_Click);
				// 
				// numericUpDown1
				// 
				this->numericUpDown1->DecimalPlaces = 3;
				this->numericUpDown1->Font = (gcnew System::Drawing::Font(L"Arial", 9.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(0)));
				this->numericUpDown1->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 25, 0, 0, 131072 });
				this->numericUpDown1->Location = System::Drawing::Point(149, 151);
				this->numericUpDown1->Name = L"numericUpDown1";
				this->numericUpDown1->Size = System::Drawing::Size(88, 26);
				this->numericUpDown1->TabIndex = 4;
				this->numericUpDown1->ThousandsSeparator = true;
				this->numericUpDown1->Visible = false;
				// 
				// checkBox1
				// 
				this->checkBox1->AutoSize = true;
				this->checkBox1->Font = (gcnew System::Drawing::Font(L"Arial", 9.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(0)));
				this->checkBox1->Location = System::Drawing::Point(53, 154);
				this->checkBox1->Name = L"checkBox1";
				this->checkBox1->Size = System::Drawing::Size(83, 22);
				this->checkBox1->TabIndex = 5;
				this->checkBox1->Text = L"Escalar";
				this->checkBox1->UseVisualStyleBackColor = true;
				this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &MyForm::checkBox1_CheckedChanged);
				// 
				// label3
				// 
				this->label3->AutoSize = true;
				this->label3->Font = (gcnew System::Drawing::Font(L"Arial", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					static_cast<System::Byte>(0)));
				this->label3->Location = System::Drawing::Point(119, 189);
				this->label3->Name = L"label3";
				this->label3->Size = System::Drawing::Size(156, 51);
				this->label3->TabIndex = 6;
				this->label3->Text = L"< 1 = Achicar modelo\r\n1 = Original\r\n> 1 = Agrandar modelo";
				this->label3->Visible = false;
				// 
				// MyForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->BackColor = System::Drawing::SystemColors::Control;
				this->ClientSize = System::Drawing::Size(630, 317);
				this->Controls->Add(this->label3);
				this->Controls->Add(this->checkBox1);
				this->Controls->Add(this->numericUpDown1);
				this->Controls->Add(this->label2);
				this->Controls->Add(this->button1);
				this->Controls->Add(this->label1);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
				this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
				this->Name = L"MyForm";
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
				this->Text = L"LAVIS-Visualizer";
				this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
	#pragma endregion
		private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
		}
		private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
			FolderBrowserDialog^ browseFolder = gcnew FolderBrowserDialog;
			browseFolder->ShowDialog();
			String^ path = browseFolder->SelectedPath;

			if (path != "") {
				std::string convPath = msclr::interop::marshal_as<std::string>(path);
				std::string str1 = "\\";
				std::string str2 = "\\\\";

				boost::replace_all(convPath, str1, str2);
				convPath.append(str2);
				
				double scale = NULL;
				if (numericUpDown1->Visible == true) {
					if (numericUpDown1->Value > 0) {
						scale = (double)numericUpDown1->Value;
					}
				}
				else {
					scale = NULL;
				}

				VTKdisplay(convPath, scale);

			}
			else {
				label2->Text = "No se encontro el destino, intentalo otra vez.";
			}
		}
	private: System::Void label4_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		if (checkBox1->Checked) {
			numericUpDown1->Show();
			label3->Show();
		}
		else
		{
			numericUpDown1->Hide();
			label3->Hide();
		}
	}
private: System::Void label2_Click(System::Object^ sender, System::EventArgs^ e) {
}
};
}

/*
*		END WINDOWS FORM CODE
*/