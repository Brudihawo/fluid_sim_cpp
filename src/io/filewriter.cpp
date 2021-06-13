// Copyright 2021 Hawo Höfer
#include "filewriter.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "simulation_domain.h"

int testLittleEndian() {
  int32_t test = 1;
  char* testdata = (char*) &test;
  if (testdata[0] == 1) {
    return 1;
  }
  return 0;
}

std::string vtk_file_header(long x, long y, std::vector<std::string> field_descriptors) {
  std::ostringstream header;
  header << "<VTKFile type=\"ImageData\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt64\">" << std::endl;
  header << "  <ImageData WholeExtent=\"0 " << x << " 0 " << y << " 0 0\" Origin=\"0 0 0\" Spacing=\"1 1 1\">" << std::endl;
  header << "    <Piece Extent=\"0 " << x << " 0 " << y << " 0 0\">" << std::endl;
  header << "      <PointData>" << std::endl;
  header << "      </PointData>" << std::endl;
  header << "      <CellData Scalars=\"test\">" << std::endl;

  // add all fields and manage offsets
  for (int i = 0; i < field_descriptors.size(); i++) {
    header << "        <DataArray type=\"Float64\" Name=\"" << field_descriptors[i];
    header << "\" format=\"appended\" RangeMin=\"0\" RangeMax=\"0\" offset=\"" << i << "\"/>" << std::endl;
  }

  header << "      </CellData>" << std::endl;
  header << "    </Piece>" << std::endl;
  header << "  </ImageData>" << std::endl;
  header << "  <AppendedData encoding=\"raw\">" << std::endl;
  header << "  _";

  return header.str();
}

std::string vtk_file_tail() {
  return "\n  </AppendedData>\n</VTKFile>\n";
}

void write_vtk(SimulationDomain& domain, std::string filename) {
  std::ofstream file;
  DomainData ddata = domain.get_data();
  uint64_t field_size = ddata.NX * ddata.NY * sizeof(double);
  uint64_t length = field_size * ddata.N_SCALAR_FIELDS;
  file.open(filename, std::ios::out | std::ios::binary);
  file << vtk_file_header(ddata.NX, ddata.NY, ddata.field_descriptors);
  file.write((char*)&length, sizeof(uint64_t));

  for (int i = 0; i < ddata.N_SCALAR_FIELDS; i++) {
    file.write((char*)ddata.fields[i].data(), field_size);
  }

  file << vtk_file_tail();
  file.close();
}
