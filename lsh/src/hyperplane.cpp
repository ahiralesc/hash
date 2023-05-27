#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <climits>
#include "hyperplane.hpp"



std::string Hyperplane::encode( Eigen::VectorXf &v ) 
{
	std::string str{};

	for(auto x : v) {            
		if( x >= 0 ) 
			str.append("1");
		else
			str.append("0");
	}

	return str;
}



//void Hyperplane::partition( Eigen::MatrixXf &H, std::unordered_map<std::string, std::vector<int>> & T)
void Hyperplane::partition( Eigen::MatrixXf &H, std::unordered_map<std::string, std::vector<shingle>> & T)
{
	reset();

	/* Partitions the set of points in R^d with random uniform hyperplanes */
	do {
		shingle v = get_shingle();
		if( v.val.size() == 0 )
			 break;

		/* Applies the random hyperplabes to point v */
		Eigen::VectorXf k = H * v.val;

		/* Let key be the binary encoding of the proyection vector k */
		std::string key = encode( k );

		/* Get the list from hash(key) and append the shingle offset */
		// T[key].push_back( v.index ); 
		T[key].push_back( v );

	} while(true);
}



void Hyperplane::preprocess()
{
	/* Initiate a random seed for the random number generator */
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1, 1);

	 /* let n be the total number of points (shingles) */
    unsigned int n = ceil( buffer.size()/shng_sz );

	/* Let k = log_{1/P2}(n), thus k = log_{10}(n) / log_{10}(1/P2) */
	k = ceil( log10(n) / log10(1/P2) );
	
	/* Let p = ln(1/P1) / ln(1/P2) */
	float p = log(1/P1) / log(1/P2);

	/* Let L be the total number of buckets, with L = n^{p} */
	l = ceil( pow(n, p) );

	/* Create L random uniform matrices and partition the R^d space*/
	for(int i=0; i<l; i++) {
		HPN n;
		n.H = Eigen::MatrixXf::Zero(k, shng_sz).unaryExpr([&](float dummy){return dis(gen);});
		partition( n.H, n.T);
		L.push_back(n);
	}
}



int Hyperplane::hamming(boost::dynamic_bitset<unsigned char> &p, boost::dynamic_bitset<unsigned char> &q) 
{
	return  (p ^ q).count();
}



void Hyperplane::search(std::vector<float> &query )
{
	//std::set<int> points;
	auto cmp = [](const shingle &p, const shingle &q) { return p.index < q.index; };
	std::set<shingle, decltype(cmp)> points;
	
	/* Preparation of the input vector */
	const Eigen::Map<Eigen::VectorXf> v(&query[0],query.size());

	/* Generation of the binary strings */
	for( HPN n : L) {
		int min_dist = INT_MAX;
		std::string min_bucket{};

		/* Compute the proyection vector k and generate the binary encoding q */
		Eigen::VectorXf k = n.H * v;
		boost::dynamic_bitset<unsigned char> q( encode( k ) );
		
		/* Extract the L hast tables T */
		for( const std::pair<const std::string, std::vector<shingle>> & T : n.T ){
			/* Find the bucket that minimizes the hamming distance */
			boost::dynamic_bitset<unsigned char> p( T.first );
			int dist = hamming(p,q);
			if( dist < min_dist) {
				min_dist = dist;
				min_bucket = T.first;
			}
		}
		/* Extract the list that minimized the hamming distance */
		//std::vector<int> & blst = n.T[min_bucket];
		std::vector<shingle> & blst = n.T[min_bucket];
		//std::copy(blst.begin(), blst.end(), std::inserter(points, points.end()));
		//for(auto point : blst)
		//	points.insert(point);
	}

	/* print the list of points */
	//std::cout << "Indexes similar to point start at locations : " << std::endl;
	//for(auto v: points)
	//	std::cout << v << ", ";
	
	// Calculate the number of vectors in the buffer based on the buffer size and shng_sz
	/* int numVectors = buffer.size() / shng_sz; */

	// Calculate distances between v and the vectors
	/* std::cout << "Distances from v to the vectors:" << std::endl;
	for (int vIndex = 0; vIndex < numVectors; vIndex++) {
    		const std::vector<std::vector<unsigned int>>& vectors = get_vectors(std::vector<unsigned int>{static_cast<unsigned int>(vIndex)});
    		for (const std::vector<unsigned int>& p : vectors) {
        		float distance = EuclidianD(v, p);  // Updated function call
        		std::cout << "Vector at index " << vIndex << ": " << distance << std::endl;
    		}
	}*/	
}


std::vector<std::vector<unsigned int>> &  Hyperplane::get_vectors(std::vector<unsigned int>& index) {
    std::vector<std::vector<unsigned int>> V;
    V.reserve(shng_sz);

    for (int i = 0; i < shng_sz; i++) {
        std::vector<unsigned int> vi;
        V.push_back(vi);
    }

    for (auto i : index) {
        int offset = i * shng_sz;
        std::vector<unsigned int> t(buffer.begin() + offset, buffer.begin() + offset + shng_sz);

        for (int j = 0; j < shng_sz; j++) {
            V[j].push_back(t[j]);
        }
    }

    return V;
}


float Hyperplane::EuclidianD(const Eigen::VectorXf& v, const std::vector<unsigned int>& p) {
    // Check if the dimensions of v and p match
    if (v.size() != p.size()) {
        // if missmatched dimenssions: throw this error
        return -1.0f;
    }

    // Compute the Euclidean distance between v and p
    float distance = 0.0f;
    for (int i = 0; i < v.size(); ++i) {
        float diff = v[i] - static_cast<float>(p[i]);
        distance += diff * diff;
    }
    distance = std::sqrt(distance);

    return distance;
}
