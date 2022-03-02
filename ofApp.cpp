#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 40) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor color) {

	this->color = color;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	// 配色デザイン ソフトグリーン P114
	this->color_palette.push_back(ofColor(97, 157, 110));
	this->color_palette.push_back(ofColor(119, 180, 106));
	this->color_palette.push_back(ofColor(143, 173, 91));
	this->color_palette.push_back(ofColor(145, 195, 161));
	this->color_palette.push_back(ofColor(93, 169, 145));
	this->color_palette.push_back(ofColor(255, 255, 255));
	this->color_palette.push_back(ofColor(131, 193, 215));
	this->color_palette.push_back(ofColor(249, 229, 128));

	ofBackground(this->color_palette.back());

	auto span = 40;
	for (int x = -280; x <= 280; x += span) {

		for (int y = -280; y <= 280; y += span) {

			this->location_list.push_back(glm::vec3(x, y, 0));
		}
	}

	auto param = span * sqrt(3);
	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= param) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	// 配色デザイン ソフトグリーン P114
	this->color_palette.push_back(ofColor(97, 157, 110));
	this->color_palette.push_back(ofColor(119, 180, 106));
	this->color_palette.push_back(ofColor(143, 173, 91));
	this->color_palette.push_back(ofColor(145, 195, 161));
	this->color_palette.push_back(ofColor(93, 169, 145));
	this->color_palette.push_back(ofColor(249, 229, 128));
	this->color_palette.push_back(ofColor(131, 193, 215));
	this->color_palette.push_back(ofColor(255, 255, 255));

	for (int i = 0; i < 120; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(this->color_palette[(int)ofRandom(this->color_palette.size() - 1)]);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 20;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWindowSize() * 0.5);

	ofSetLineWidth(3);
	for (auto& actor : this->actor_list) {

		ofSetColor(actor->getColor());

		if (actor->getLog().size() > 2) {

			if (actor->getLog()[0] == actor->getLog()[1]) {

				ofFill();
				ofDrawCircle(actor->getLocation(), 6);
			}
			else {

				auto angle = std::atan2(actor->getLog()[0].y - actor->getLog()[1].y, actor->getLog()[0].x - actor->getLog()[1].x);
				auto deg_start = angle * RAD_TO_DEG;

				ofFill();
				ofBeginShape();
				for (int deg = deg_start; deg < deg_start + 360; deg += 120) {

					ofVertex(actor->getLocation() + glm::vec2(6 * cos(deg * DEG_TO_RAD), 6 * sin(deg * DEG_TO_RAD)));
				}
				ofEndShape(true);
			}
		}

		ofNoFill();
		ofBeginShape();
		for (auto& l : actor->getLog()) {

			ofVertex(l);
		}
		ofEndShape();
	}
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}