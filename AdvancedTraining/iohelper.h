#pragma once
template<typename T>
void write_pod(std::ofstream& out, T& t)
{
	out.write(reinterpret_cast<char*>(&t), sizeof(T));
}

template<typename T>
void read_pod(std::ifstream& in, T& t)
{
	in.read(reinterpret_cast<char*>(&t), sizeof(T));
}

template<typename T>
void read_pod_vector(std::ifstream& in, std::vector<T>& vect)
{
	long size;
	read_pod(in, size);
	vect.resize(size);
	in.read(reinterpret_cast<char*>(&(*vect.begin())), size * sizeof(T));
}

template<typename T>
void write_pod_vector(std::ofstream& out, std::vector<T>& vect)
{
	long size = vect.size();
	write_pod<long>(out, size);
	for (auto it = vect.begin(); it != vect.end(); it++) {
		write_pod<T>(out, *it);
	}
}