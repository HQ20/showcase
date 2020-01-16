pragma solidity ^0.5.10;


/**
 * @title Simple Storage
 * @dev A simple way to a camera registry
 */
contract SimpleStorage {
    mapping(uint256 => string[]) public cameraPictures;
    uint256[] public camerasId;

    event NewCamera(uint256 cameraId);
    event NewPicture(uint256 cameraId, string ipfsHash);

    /**
     * empty constructor
     */
    constructor() public {
        //
    }

    /**
     * @dev Register new camera
     * @param _cameraId The camera id to register
     */
    function addCamera(uint256 _cameraId) public {
        camerasId.push(_cameraId);
        emit NewCamera(_cameraId);
    }

    /**
     * @dev Add picture to camera's registry
     * @param _cameraId The camera id from where the picture was taken
     * @param _ipfsHash The IPFS hash of the added picture
     */
    function addPicture(uint256 _cameraId, string memory _ipfsHash) public {
        cameraPictures[_cameraId].push(_ipfsHash);
        emit NewPicture(_cameraId, _ipfsHash);
    }

    /**
     * @dev Gets the total number of cameras regitered
     */
    function totalCameras() public view returns (uint256) {
        return camerasId.length;
    }

    /**
     * @dev Total number of pictures of a given camera
     */
    function totalPictures(uint256 _cameraId) public view returns (uint256) {
        return cameraPictures[_cameraId].length;
    }
}
