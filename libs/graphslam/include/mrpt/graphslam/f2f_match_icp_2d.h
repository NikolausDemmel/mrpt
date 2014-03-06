/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */
#ifndef GRAPH_SLAM_F2FMATCH_ICP2D_H
#define GRAPH_SLAM_F2FMATCH_ICP2D_H

#include <mrpt/graphslam/types.h>
#include <mrpt/utils/CLoadableOptions.h>
#include <mrpt/slam/CICP.h>
#include <mrpt/graphslam/link_pragmas.h>

namespace mrpt { namespace graphslam { namespace f2f_match {
	using mrpt::utils::TNodeID; 

	/** GraphSLAM Frame-to-frame match finder. See mrpt::graphslam::GraphSlamEngine<> */
	struct GRAPHSLAM_IMPEXP GS_F2F_ICP_2D
	{
		/** Returns true to save the current frame; return false to overwrite it with more recent data and continue updating its pose */
		template <class GRAPHSLAMENGINE>
		void getCovisibleKeyframes(
			const GRAPHSLAMENGINE &gse, 
			const TNodeID curKeyFrameID, 
			std::set<TNodeID> &out_covis_KFs)
		{
			out_covis_KFs.clear();

			// Current pose:
			typename GRAPHSLAMENGINE::pose_t cur_pose;
			gse.getCurrentPose(cur_pose);

			// Get KD-tree to search for neaby keyframes close to the current pose:
			const double maxRadiusSqr = mrpt::utils::square( params.kf2kf_max_search_radius );
			std::vector<std::pair<size_t,double> > nearbyKFs;

			const typename GRAPHSLAMENGINE::KeyFramesKDTree &kdtree = gse.getKeyFrameKDTree();
			if (GRAPHSLAMENGINE::rotation_dimensions==3)
			     kdtree.kdTreeRadiusSearch3D (cur_pose[0], cur_pose[1], cur_pose[2], maxRadiusSqr,nearbyKFs);
			else kdtree.kdTreeRadiusSearch2D (cur_pose[0], cur_pose[1], maxRadiusSqr,nearbyKFs);

			MRPT_TODO("Also check orientations?")
			for (size_t i=0;i<nearbyKFs.size();i++) 
				if (nearbyKFs[i].first!=curKeyFrameID)
					out_covis_KFs.insert( nearbyKFs[i].first );

		} // end of getCovisibleKeyframes


		/** Keyframe-to-keyframe match method.
		  * \return true if a valid registration was found.
		  */
		bool matchTwoKeyframes(
			const TNodeID id_a, const TNodeID id_b, 
			const CSensoryFrame &obs_a, const CSensoryFrame &obs_b,
			const mrpt::poses::CPose2D &approx_pose_b_from_a,
			mrpt::poses::CPose2D &out_pose_b_from_a );


		struct GRAPHSLAM_IMPEXP TParams : public mrpt::utils::CLoadableOptions
		{
			// ...
			double kf2kf_max_search_radius; //!< Maximum radius for search of candidate KFs to loop closure / edge creation (meters).

			mrpt::slam::CICP::TConfigParams icp_params; //!< All the parameters of the ICP algorithm

			TParams(); 
			virtual void loadFromConfigFile(const mrpt::utils::CConfigFileBase	&source,const std::string &section);
			virtual void dumpToTextStream(mrpt::utils::CStream &out) const;
		};

		TParams params;

		GS_F2F_ICP_2D();  //!< Default ctor
		

	}; // end GS_F2F_ICP_2D

}}} // end namespaces


#endif
