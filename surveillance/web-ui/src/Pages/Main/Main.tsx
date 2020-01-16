import { ethers } from 'ethers';
import { JsonRpcProvider } from 'ethers/providers';
import React, { useEffect, useState } from 'react';

import {
    Container,
    Grid,
    List,
    ListItem,
    ListItemIcon,
    ListItemText,
    makeStyles,
    Paper,
    Typography,
} from '@material-ui/core';
import FolderIcon from '@material-ui/icons/Folder';

import SimpleStorageContractJSON from 'smart-contracts/build/contracts/SimpleStorage.json';
import { SimpleStorageInstance } from 'smart-contracts/types/truffle-contracts/index';


const useStyles = makeStyles((theme) => ({
    image: {
        height: 128,
        width: 128,
    },
    img: {
        display: 'block',
        margin: 'auto',
        maxHeight: '100%',
        maxWidth: '100%',
    },
    paper: {
        margin: 'auto',
        maxWidth: 850,
        padding: theme.spacing(2),
    },
    root: {
        flexGrow: 1,
    },
}));
interface IMainState {
    camerasId: string[];
    userSigner: ethers.providers.JsonRpcSigner;
    provider: JsonRpcProvider;
    simpleStorageInstance: ethers.Contract & SimpleStorageInstance;
}
function Main() {
    const [camerasId, setCamerasId] = useState<string[]>([]);
    const [cameraPictures, setCameraPictures] = useState<string[]>([]);
    const [provider, setProvider] = useState<JsonRpcProvider>(undefined as any);
    const [simpleStorageInstance, setSimpleStorageInstance] =
        useState<ethers.Contract & SimpleStorageInstance>(undefined as any);
    const [selectedCamera, setSelectedCamera] = useState<number>(-1);

    const classes = useStyles();

    useEffect(() => {
        const fetchData = async () => {
            const url = 'http://localhost:8545';
            const customHttpProvider = new ethers.providers.JsonRpcProvider(url);

            // We connect to the Contract using a Provider, so we will only
            // have read-only access to the Contract
            const network = await customHttpProvider.getNetwork();
            const newSimpleStorageInstance = new ethers.Contract(
                // TODO: improve next line
                (SimpleStorageContractJSON.networks as any)[network.chainId].address,
                SimpleStorageContractJSON.abi,
                customHttpProvider,
            ) as ethers.Contract & SimpleStorageInstance;

            const totalCameras = (await newSimpleStorageInstance.totalCameras()).toNumber();
            const newCamerasId: string[] = [];
            for (let c = 0; c < totalCameras; c += 1) {
                newCamerasId.push((await newSimpleStorageInstance.camerasId(c)).toString());
            }

            // Set provider and contract to the state, and then proceed with an
            // example of interacting with the contract's methods.
            setProvider(customHttpProvider);
            setSimpleStorageInstance(newSimpleStorageInstance);
            setCamerasId(newCamerasId);
        };
        fetchData();
    }, []);

    const handleListItemClick = (event: React.MouseEvent<HTMLDivElement, MouseEvent>) => {
        const cameraId = parseInt(event.currentTarget.id, 10);
        simpleStorageInstance.totalPictures(cameraId).then(async (totalPictures) => {
            const pictures = [];
            for (let p = 0; p < totalPictures.toNumber(); p += 1) {
                const cameraResult = await simpleStorageInstance.cameraPictures(cameraId, p);
                console.log(cameraResult);
                pictures.push(cameraResult);
            }
            setCameraPictures(pictures);
            setSelectedCamera(cameraId);
        });
    };

    const render = () => {
        if (!provider) {
            return <div>Loading Web3, accounts, and contract...</div>;
        }
        let content;
        let title;
        if (selectedCamera !== -1) {
            title = 'Viewing files of ' + selectedCamera;
            const cameraPicturesContent = cameraPictures.map((picture) => <Grid key={picture} item={true} xs={3}>
                <Paper className={classes.paper}>
                    <img src={'http://localhost:8080/ipfs/' + picture} width="150" />
                </Paper>
            </Grid>);
            content = <Grid container={true} spacing={3}>
                {cameraPicturesContent}
            </Grid>;
        } else {
            title = 'Select one camera by the ID';
            content = camerasId.map((camera) => (
                <ListItem
                    button={true}
                    key={camera}
                    id={camera}
                    onClick={handleListItemClick}
                >
                    <ListItemIcon>
                        <FolderIcon />
                    </ListItemIcon>
                    <ListItemText
                        primary={camera}
                    />
                </ListItem >
            ));
        }
        return (
            <div className={classes.root}>
                <Paper className={classes.paper}>
                    <Container maxWidth="md">
                        <Typography variant="h4">{title}</Typography>
                        <List dense={true}>
                            {content}
                        </List>
                    </Container>
                </Paper>
            </div>
        );
    };
    return render();
}

export default Main;
